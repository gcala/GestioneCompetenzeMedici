#include "differenzedialog.h"
#include "ui_differenzedialog.h"
#include "sqlqueries.h"
#include "utilities.h"

#include <QFileDialog>

DifferenzeDialog::DifferenzeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DifferenzeDialog)
{
    ui->setupUi(this);

    connect(ui->meseCB, &QComboBox::currentIndexChanged, this, &DifferenzeDialog::meseCurrentIndexChanged);
    connect(ui->saveButton, &QPushButton::clicked, [=] () {
        proceed = true;
        close();
    });
    connect(ui->cancelButton, &QPushButton::clicked, [=] () {
        proceed = false;
        close();
    });
}

DifferenzeDialog::~DifferenzeDialog()
{
    delete ui;
}

void DifferenzeDialog::addMese(const QString &mese, const QVariant &id)
{
    ui->meseCB->addItem(mese, id);
}

void DifferenzeDialog::clearMese()
{
    ui->meseCB->clear();
}

QString DifferenzeDialog::currentMeseData() const
{
    return ui->meseCB->currentData().toString();
}

int DifferenzeDialog::currentUnitaData() const
{
    return ui->unitaCB->currentData().toInt();
}

void DifferenzeDialog::setCurrentMese(int index)
{
    ui->meseCB->setCurrentIndex(index);
}

void DifferenzeDialog::setCurrentUnita(int index)
{
    ui->unitaCB->setCurrentIndex(index);
}

QString DifferenzeDialog::path() const
{
    return ui->path->text();
}

void DifferenzeDialog::setPath(const QString &path)
{
    ui->path->setText(path);
}

bool DifferenzeDialog::storicizzaIsChecked() const
{
    return ui->casiCB->isChecked();
}

bool DifferenzeDialog::pdfIsChecked() const
{
    return ui->dataCB->isChecked();
}

void DifferenzeDialog::meseCurrentIndexChanged(int index)
{
    Q_UNUSED(index)

    ui->unitaCB->clear();
    ui->unitaCB->addItem("Tutte", -1);

    if(ui->meseCB->currentData(Qt::UserRole).toString().isEmpty())
        return;

    QStringList query = SqlQueries::getUnitaDataFromTimecard(ui->meseCB->currentData(Qt::UserRole).toString());

    QStringList list;

    for(const QString &s : query) {
        QStringList l = s.split("~");
        if(!list.contains(l.at(1))) {
            ui->unitaCB->addItem(l.at(2) + " - " + l.at(1), l.at(0));
            list << l.at(1);
        }
    }
}

void DifferenzeDialog::browseButtonClicked()
{
    QString dir = QFileDialog::getExistingDirectory( this,
                                                     tr( "Seleziona destinazione" ),
                                                     Utilities::m_exportPath,
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );

    // be sure that a valid path was selected
    const QFileInfo fi(dir);
    if( fi.exists() ) {
        Utilities::m_exportPath = fi.absoluteFilePath();
        ui->path->setText( dir );
    }
}

