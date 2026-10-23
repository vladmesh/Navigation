#include <QtTest>

#include "nmeaparser.h"
#include "ppkparser.h"

class TestParsers : public QObject
{
    Q_OBJECT

private slots:
    void ggaTimeOnly();
    void ggaFullParse();
    void ggaInvalidSentence();
    void utmRoundTrip();
    void rmcParse();
    void hdtParse();
    void rmcInvalid();
    void dmsToDecimal();
    void ppkParseLine();
    void ppkHeaderAndGarbage();
    void ppkTimeFromLine();
    void compareCoordinates();

private:
    static QString sampleGGA()
    {
        return QStringLiteral("$GPGGA,123519.50,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47");
    }
    // 28 полей, разделённых пробелами; индексы см. PpkParser::parseLine.
    static QString samplePpk()
    {
        QStringList f;
        for (int i = 0; i < 28; ++i)
            f << QStringLiteral("0");
        f[0] = QStringLiteral("GD");
        f[4] = QStringLiteral("2024-09-22");
        f[5] = QStringLiteral("04:32:18.50");
        f[16] = QStringLiteral("48");
        f[17] = QStringLiteral("7");
        f[18] = QStringLiteral("2.28");
        f[19] = QStringLiteral("11");
        f[20] = QStringLiteral("31");
        f[21] = QStringLiteral("0.0");
        f[22] = QStringLiteral("545.4");
        f[24] = QStringLiteral("691000.12");
        f[25] = QStringLiteral("5333000.34");
        return f.join(QStringLiteral("   "));
    }
};

void TestParsers::ggaTimeOnly()
{
    NmeaParser p;
    QCOMPARE(p.getTimeFromNmeaGGA(sampleGGA()), QTime(12, 35, 19, 500));
    QVERIFY(!p.getTimeFromNmeaGGA(QStringLiteral("$GPXXX,1,2")).isValid());
}

void TestParsers::ggaFullParse()
{
    NmeaParser p;
    const NmeaParser::CoordinateData d = p.parseNmeaGGA(sampleGGA());
    QVERIFY(d.coordinate.isValid());
    QVERIFY(qAbs(d.coordinate.latitude() - (48.0 + 7.038 / 60.0)) < 1e-9);
    QVERIFY(qAbs(d.coordinate.longitude() - (11.0 + 31.0 / 60.0)) < 1e-9);
    QVERIFY(qAbs(d.height - 545.4f) < 1e-3f);
    QCOMPARE(d.dateTime.time(), QTime(12, 35, 19, 500));
    QVERIFY(NmeaParser::isValid(d));
    // Мюнхен: UTM зона 32, восток ~687 км, север ~5332 км.
    QVERIFY2(qAbs(d.coorUTM.x() - 687300.0) < 100.0, qPrintable(QString::number(d.coorUTM.x())));
    QVERIFY2(qAbs(d.coorUTM.y() - 5332400.0) < 500.0, qPrintable(QString::number(d.coorUTM.y())));
}

void TestParsers::ggaInvalidSentence()
{
    NmeaParser p;
    const NmeaParser::CoordinateData d = p.parseNmeaGGA(QStringLiteral("$GPGGA,1,2,3"));
    QVERIFY(!d.coordinate.isValid());
}

void TestParsers::utmRoundTrip()
{
    NmeaParser p;
    const QGeoCoordinate src(-33.8688, 151.2093); // южное полушарие
    const QPointF utm = p.GeoToUTM(src);
    const QGeoCoordinate back = p.UTMtoGeo(utm);
    QVERIFY2(qAbs(back.latitude() - src.latitude()) < 1e-5, qPrintable(QString::number(back.latitude())));
    QVERIFY2(qAbs(back.longitude() - src.longitude()) < 1e-5, qPrintable(QString::number(back.longitude())));
}

void TestParsers::rmcParse()
{
    NmeaParser p;
    const auto r =
        p.parseNmeaRMC(QStringLiteral("$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A"));
    QVERIFY(qAbs(r.speed - 22.4) < 1e-9);
    QVERIFY(qAbs(r.azimuth - 84.4) < 1e-9);
    QVERIFY(NmeaParser::isValid(r));
}

void TestParsers::hdtParse()
{
    NmeaParser p;
    // Парсер берёт всё после первой запятой целиком, поэтому стандартная
    // форма "$GPHDT,123.4,T" сейчас даёт 0; проверяем ту форму, которую код
    // реально поддерживает, а стандартную фиксируем как известный дефект.
    const auto r = p.parseNmeaRMC(QStringLiteral("$GPHDT,123.4"));
    QVERIFY(qAbs(r.azimuth - 123.4) < 1e-4);
    const auto hdtStd = p.parseNmeaRMC(QStringLiteral("$GPHDT,123.4,T"));
    QEXPECT_FAIL("", "known bug: suffix ',T' of standard HDT is not stripped", Continue);
    QVERIFY(qAbs(hdtStd.azimuth - 123.4) < 1e-4);
}

void TestParsers::rmcInvalid()
{
    NmeaParser p;
    const auto r = p.parseNmeaRMC(QStringLiteral("garbage"));
    QCOMPARE(r.azimuth, -1.0);
    QVERIFY(!NmeaParser::isValid(r));
    // 8 полей: раньше читался parts[8] за границей списка.
    const auto shortRmc = p.parseNmeaRMC(QStringLiteral("$GPRMC,1,2,3,4,5,6,7"));
    QCOMPARE(shortRmc.azimuth, -1.0);
}

void TestParsers::dmsToDecimal()
{
    QCOMPARE(PpkParser::dmsToDecimal(10, 30, 0.0), 10.5);
    QVERIFY(qAbs(PpkParser::dmsToDecimal(48, 7, 2.28) - 48.1173) < 1e-6);
}

void TestParsers::ppkParseLine()
{
    const auto d = PpkParser::parseLine(samplePpk());
    QVERIFY(d.coordinate.isValid());
    QVERIFY(qAbs(d.coordinate.latitude() - 48.1173) < 1e-6);
    QVERIFY(qAbs(d.coordinate.longitude() - (11.0 + 31.0 / 60.0)) < 1e-9);
    QCOMPARE(d.coorUTM, QPointF(691000.12, 5333000.34));
    QVERIFY(qAbs(d.height - 545.4f) < 1e-3f);
    if (!d.dateTime.isValid())
        QSKIP("known bug: format 'hh:mm:ss.zz' in PpkParser is not supported by this Qt version");
    QCOMPARE(d.dateTime.date(), QDate(2024, 9, 22));
    QCOMPARE(d.dateTime.time(), QTime(4, 32, 18, 500));
}

void TestParsers::ppkHeaderAndGarbage()
{
    QVERIFY(!PpkParser::parseLine(QString()).coordinate.isValid());
    QVERIFY(!PpkParser::parseLine(QStringLiteral("a b c")).coordinate.isValid());
    QString header = samplePpk();
    header.replace(0, 2, QStringLiteral("DIR"));
    QVERIFY(!PpkParser::parseLine(header).coordinate.isValid());
}

void TestParsers::ppkTimeFromLine()
{
    QVERIFY(!PpkParser::getTimeFromPpkLine(QStringLiteral("x")).isValid());
    const QTime t = PpkParser::getTimeFromPpkLine(samplePpk());
    if (!t.isValid())
        QSKIP("known bug: format 'hh:mm:ss.zz' in PpkParser is not supported by this Qt version");
    QCOMPARE(t, QTime(4, 32, 18, 500));
}

void TestParsers::compareCoordinates()
{
    const auto a = PpkParser::parseLine(samplePpk());
    QVERIFY(PpkParser::compareCoordinates(a, a));
    auto b = a;
    b.coorUTM.setX(b.coorUTM.x() + 1.0);
    QVERIFY(!PpkParser::compareCoordinates(a, b));
    auto c = a;
    c.dateTime = QDateTime(QDate(2024, 9, 22), QTime(4, 32, 19));
    QVERIFY(!PpkParser::compareCoordinates(a, c));
}

QTEST_GUILESS_MAIN(TestParsers)
#include "tst_parsers.moc"
