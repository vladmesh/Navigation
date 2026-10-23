#ifndef SOURCEDIALOG_H
#define SOURCEDIALOG_H

#include <buoydialog.h>

class SourceDialog : public BuoyDialog
{
    Q_OBJECT
public:
    SourceDialog() : BuoyDialog(nullptr, false){
        setWindowTitle("Source Parametrs");
        spinBox2->setValue(0.15);
        spinBoxLabel2->setText("Source depth");
    };
};

#endif // SOURCEDIALOG_H
