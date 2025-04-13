#include "differenzedialog.h"
#include "ui_differenzedialog.h"
//#include "sqlqueries.h"
#include "utilities.h"
#include "apiservice.h"

#include <QFileDialog>

DifferenzeDialog::DifferenzeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DifferenzeDialog)
{
    ui->setupUi(this);

    connect(ui->meseCB, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DifferenzeDialog::meseCurrentIndexChanged);
    connect(ui->browseButton, &QPushButton::clicked, this, &DifferenzeDialog::browseButtonClicked);
    connect(ui->saveButton, &QPushButton::clicked, [=] () {
        proceed = true;
        close();
    });
    connect(ui->cancelButton, &QPushButton::clicked, [=] () {
        proceed = false;
        close();
    });
    const auto now = QDateTime::currentDateTime();
    if(now.date().day()>=20) {
        ui->meseAnnoPag->setDate(now.date().addMonths(1));
    } else {
        ui->meseAnnoPag->setDate(now.date());
    }
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

int DifferenzeDialog::mesePagamento() const
{
    return ui->meseAnnoPag->date().month();
}

int DifferenzeDialog::annoPagamento() const
{
    return ui->meseAnnoPag->date().year();
}

void DifferenzeDialog::meseCurrentIndexChanged(int index)
{
    Q_UNUSED(index)

    ui->unitaCB->clear();
    ui->unitaCB->addItem("Tutte", -1);

    if(ui->meseCB->currentData(Qt::UserRole).toString().isEmpty())
        return;

    const auto unitaData = ApiService::instance().getUnitaDataFromTimecard(ui->meseCB->currentData(Qt::UserRole).toString());

    QStringList list;

    for(const auto unita : unitaData) {
        if(!list.contains(unita.nome)) {
            ui->unitaCB->addItem(QString::number(unita.idUnita) + " - " + unita.nome, unita.id);
            list << unita.nome;
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

