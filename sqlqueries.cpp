/*
 * Gestione Competenze Medici
 *
 * Copyright (C) 2017 Giuseppe Calà <giuseppe.cala@mailbox.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "sqlqueries.h"
#include "dipendente.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

QMap<int, QStringList> SqlQueries::m_unitsMap;

void SqlQueries::createUnitsTable()
{
    QSqlQuery query;
    query.prepare("CREATE TABLE unita "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                  "raggruppamento TEXT,"
                  "nome_full TEXT,"
                  "nome_mini TEXT,"
                  "numero TEXT,"
                  "altri_nomi TEXT);");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::createDoctorsTable()
{
    QSqlQuery query;
    query.prepare("CREATE TABLE medici "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                  "matricola INTEGER,"
                  "nome TEXT,"
                  "id_unita INTEGER);");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::createUnitsPayedHoursTable()
{
    QSqlQuery query;
    query.prepare("CREATE TABLE unita_ore_pagate "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                  "id_unita INTEGER,"
                  "ore INTEGER NOT NULL DEFAULT (0),"
                  "data TEXT DEFAULT (01.2017));");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::createUnitsRepTable()
{
    QSqlQuery query;
    query.prepare("CREATE TABLE unita_reperibilita "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                  "id_unita INTEGER NOT NULL,"
                  "data TEXT NOT NULL DEFAULT (01.2017),"
                  "feriale REAL NOT NULL DEFAULT (0.0),"
                  "sabato REAL NOT NULL DEFAULT (0.0),"
                  "prefestivo REAL NOT NULL DEFAULT (0.0),"
                  "festivo REAL NOT NULL DEFAULT (0.0));");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::populateUnitsTable()
{
    QSqlQuery query;
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('1', 'A010201', 'U.O.C. DIREZIONE SANITARIA AZIENDALE', 'DIREZIONE SANITARIA', '1', 'Direzione Sanitaria~Direzione Medica~Epidemiologia e Programmazione~U.O.C. Igiene Ospedaliera~U.O.C. Qualità Risk Management');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('2', 'A040101', 'U.O.C. ACCETTAZIONE PRONTO SOCCORSO E MEDICINA DI URGENZA', 'ACCETTAZIONE PRONTO SOCCORSO E MEDICINA DI URGENZA', '2', 'U.O.C. Accet. Pronto Soc. Med. Urg.~U.O.C. Accett. Pronto Soccorso e Med~U.O.C. Accettaz., PS, Urgenza~U.O.C. Accett. Pronto Soccorso e Me~U.O.C. Accett. Pronto Soccorso e M~Pronto Soccorso~U.O.C. Accett. Pronto Soccorso e M');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('3', 'A070107', 'U.O.C. ANATOMIA PATOLOGICA E CITODIAGNOSTICA', 'ANATOMIA PATOLOGICA', '3', 'Anatomia e citodiagnostica');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('4', 'A040102', 'U.O.C. ANESTESIA e RIANIMAZIONE', 'ANESTESIA e RIANIMAZIONE', '4', 'Anest.Rianim.~Anestesia e Rianimazione~U.O.C. Anestesia e Rianimazione');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('5', 'A050103', 'U.O.C. CARDIOLOGIA EMODINAMICA', 'CARDIOLOGIA EMODINAMICA', '5', 'U.O.C. Cardiologia Emodinamica');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('6', 'A050102', 'U.O.C. CARDIOLOGIA MEDICA', 'CARDIOLOGIA MEDICA', '6', 'U.O.C. Cardiologia Medica');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('7', 'A050108', 'S.S.D. CARDIOLOGIA RIABILITATIVA', 'CARDIOLOGIA RIABILITATIVA', '7', 'S.S.D. Cardiologia Riabilitativa');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('8', 'A060110', 'S.S.D. DIABETOLOGIA', 'DIABETOLOGIA', '8', 'S.S.D. Diabetologia');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('9', 'A070106', 'U.O.C. SERVIZIO IMMUNOTRASFUSIONALE', 'SERVIZIO IMMUNOTRASFUSIONALE', '9', 'Servizio Immunotrasfusionale');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('10', 'A050201', 'U.O.C. CHIRURGIA GENERALE', 'CHIRURGIA GENERALE', '10', 'U.O.C. Chirurgia Generale');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('11', 'A050202', 'U.O.C. CHIRURGIA SENOLOGICA E PLASTICA', 'SENOLOGIA', '11', 'U.O.C. Chirurgia Senologica e Plastica~U.O.C. Chirurgia Senologica e Plas~U.O.C. Chirurgia Senologica e Plast~U.O.C. Chirurgia Senologica e Plasti');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('12', 'A050101', 'U.O.C. CARDIOCHIRURGIA', 'CARDIOCHIRURGIA', '12', 'U.O.C. Cardiochirurgia');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('13', 'A040103', 'U.O.C. CARDIOANESTESIA E RIANIMAZIONE CARDIOLOGICA', 'CARDIOANESTESIA', '13', 'U.O.C. Cardioanestesia e Rianimazione~U.O.C. Cardio e Rian~U.O.C. Cardio e Ria~U.O.C. Cardio e Ri');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('14', 'A040106', 'U.O.C. CHIRURGIA DI URGENZA', 'CHIRURGIA D''URGENZA', '14', 'U.O.C. Chirurgia d''Urgenza');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('15', 'A050301', 'U.O.C. ONCOLOGIA MEDICA', 'ONCOLOGIA', '15', 'U.O.C. Oncologia Medica');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('16', 'A050302', 'U.O.C. EMATOLOGIA', 'EMATOLOGIA', '16', 'U.O.C. Ematologia');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('17', 'A060105', 'U.O.C. NEFROLOGIA E DIALISI', 'NEFROLOGIA E DIALISI', '17', 'U.O.C. Nefrologia e Dialisi');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('18', 'A060106', 'U.O.C. ENDOCRINOLOGIA', 'ENDOCRINOLOGIA', '18', 'U.O.C. Endocrinologia');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('19', 'A060104', 'U.O.C. GERIATRIA', 'GERIATRIA', '19', 'U.O.C. Geriatria');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('20', 'A070105', 'LABORATORIO ANALISI', 'LABORATORIO ANALISI', '20', 'Laboratorio Analisi~Laboratorio d''Urgenza');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('21', 'A060101', 'U.O.C. MEDICINA INTERNA', 'MEDICINA INTERNA', '21', 'Medicina Interna');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('22', 'A050304', 'U.O.C. MALATTIE INFETTIVE', 'MALATTIE INFETTIVE', '22', 'Malattie Infettive');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('23', 'A030110', 'S.S. DI U.O.C. MEDICINA DEL LAVORO', 'MEDICINA DEL LAVORO', '23', 'S.S. di UOC Medicina del Lavoro');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('24', 'A060202', 'U.O.C. NEONATOLOGIA', 'NEONATOLOGIA', '24', 'Neonatologia');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('25', 'A050205', 'U.O.C. NEUROCHIRURGIA', 'NEUROCHIRURGIA', '25', 'U.O.C. Neurochirurgia');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('26', 'A060102', 'U.O.C. NEUROLOGIA', 'NEUROLOGIA', '26', 'U.O.C. Neurologia~Neurologia~Stroke Unit');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('27', 'A050206', 'U.O.C. OCULISTICA', 'OCULISTICA', '27', 'U.O.C. Oculistica');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('28', 'A050213', 'S.S.D. LITOTRISSIA E CENTRO PER LO STUDIO DELLA CALCOLOSI RENALE', 'LITOTRISSIA', '28', 'Litotrissia');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('29', 'A040105', 'U.O.C. TRAUMATOLOGIA', 'TRAUMATOLOGIA', '29', 'U.O.C. Traumatologia');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('30', 'A060201', 'U.O.C. OSTETRICIA E GINECOLOGIA', 'OSTETRICIA E GINECOLOGIA', '30', 'Ostetricia e Ginecologia');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('31', 'A050207', 'U.O.C. OTORINOLARINGOIATRIA', 'OTORINO', '31', 'U.O.C. Otorinolaringoiatria');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('32', 'A060203', 'U.O.C. PEDIATRIA', 'PEDIATRIA', '32', 'Pediatria~Centro Cura Fibrosi Cistica');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('33', 'A060204', 'U.O.C. PNEUMOLOGIA', 'PNEUMOLOGIA', '33', 'U.O.C. Pneumologia');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('34', 'A070101', 'U.O.C. RADIOLOGIA', 'RADIOLOGIA', '34', 'Radiologia~Radiologia Intervent. e vascol.');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('35', 'A070104', 'U.O.C. MEDICINA NUCLEARE', 'MEDICINA NUCLEARE', '35', 'Medicina Nucleare');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('36', 'A050203', 'U.O.C. UROLOGIA', 'UROLOGIA', '36', 'U.O.C. Urologia~S.S.D. Andrologia');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('37', 'A050104', 'U.O.C. CARDIOLOGIA UTIC E CARDIOSTIMOLAZIONE', 'UTIC', '37', 'S.S.D. Elettrofis. interv.~U.O.C. Cardiol  UTIC e Cardiostimola~U.O.C. Cardiol UTIC e Cardiostimolazione~U.O.C. Cardiol  UTIC e Cardiostimol~U.O.C. Cardiol UTIC e Cardiostimolazion~U.O.C. Cardiol  UTIC e Cardiostimo');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('38', 'A050306', 'S.S.D. HOSPICE', 'HOSPICE', '38', 'S.S.D. Hospice~S.S.D.Hospice');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('39', 'A050204', 'U.O.C. ORTOPEDIA', 'ORTOPEDIA', '39', 'U.O.C. Ortopedia');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('40', 'A050106', 'S.S.D. COORDINAMENTO ATTIVITÀ SPECIALISTICHE AMBULATORIALI', 'COORDINAMENTO ATTIVITÀ SPECIALISTICHE AMBULATORIALI', '40', 'S.S.D. Coord. att. special.');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('41', 'A010301', 'U.O. CONTROLLO DI GESTIONE', 'CONTROLLO DI GESTIONE', '41', 'U.O.C. Controllo di Gestione');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('42', 'A050209', 'U.O.C. CHIRURGIA TORACICA', 'CHIRURGIA TORACICA', '42', 'U.O.C. Chirurgia Toracica');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('43', 'A050211', 'S.S.D. DAY SURGERY', 'DAY SURGERY', '43', 'S.S.D.  Day Surgery');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('44', 'A050105', 'U.O.C. CHIRURGIA VASCOLARE', 'CHIRURGIA VASCOLARE', '44', 'U.O.C. Chirurgia Vascolare');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('45', 'A070102', 'U.O.C. RADIOLOGIA DI URGENZA', 'RADIOLOGIA D''URGENZA', '45', 'Radiologia d''Urgenza');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('46', 'A030104', 'U.O.C. MEDICINA LEGALE', 'MEDICINA LEGALE', '46', 'Medicina Legale');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('47', 'A050303', 'U.O.C. REUMATOLOGIA', 'REUMATOLOGIA', '47', 'U.O.C. Reumatologia');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('48', 'A050409', 'S.S. DI U.O.C. MEDICINA FISICA E RIABILITATIVA', 'MEDICINA FISICA E RIABILITATIVA', '48', 'S.S. di U.O.C. Medicina fis e riabil  Po~S.S. di U.O.C. Medicina fis e riabil  P~S.S. di U.O.C. Medicina fis e riabil');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('49', 'A050305', 'U.O.C. DERMATOLOGIA E M.T.S.', 'DERMATOLOGIA', '49', 'U.O.C. Dermatologia e MTS');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('50', 'A050210', 'U.O.C. ENDOSCOPIA', 'ENDOSCOPIA', '50', 'Endoscopia Digestiva');");
    query.exec("INSERT INTO unita (\"id\", \"raggruppamento\", \"nome_full\", \"nome_mini\", \"numero\", \"altri_nomi\") VALUES ('51', 'A070210', 'U.O.C. NEURORADIOLOGIA', 'NEURORADIOLOGIA', '51', 'Neuroradiologia');");
}

void SqlQueries::populateUnitsPayedHoursTable()
{
    QSqlQuery query;
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('1', '1', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('2', '2', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('3', '3', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('4', '4', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('5', '5', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('6', '6', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('7', '7', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('8', '8', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('9', '9', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('10', '10', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('11', '11', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('12', '12', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('13', '13', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('14', '14', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('15', '15', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('16', '16', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('17', '17', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('18', '18', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('19', '19', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('20', '20', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('21', '21', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('22', '22', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('23', '23', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('24', '24', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('25', '25', '5', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('26', '26', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('27', '27', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('28', '28', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('29', '29', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('30', '30', '3', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('31', '31', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('32', '32', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('33', '33', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('34', '34', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('35', '35', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('36', '36', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('37', '37', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('38', '38', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('39', '39', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('40', '40', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('41', '41', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('42', '42', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('43', '43', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('44', '44', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('45', '45', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('46', '46', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('47', '47', '6', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('48', '48', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('49', '49', '0', '01.2017');");
    query.exec("INSERT INTO unita_ore_pagate (\"id\", \"id_unita\", \"ore\", \"data\") VALUES ('50', '50', '0', '01.2017');");
}

void SqlQueries::populateUnitsRepTable()
{
    QSqlQuery query;
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('1', '3', '1.2017', '1', '0', '1.5', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('2', '4', '1.2017', '1.5', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('3', '13', '1.2017', '1', '2', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('4', '12', '1.2017', '1', '2', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('5', '44', '1.2017', '1.5', '2', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('6', '5', '1.2017', '1', '2', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('7', '7', '1.2017', '0', '0', '0', '0');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('8', '6', '1.2017', '0', '0', '0', '0');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('9', '9', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('10', '14', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('11', '10', '1.2017', '1', '1.5', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('12', '11', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('13', '42', '1.2017', '1.5', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('14', '1', '1.2017', '1', '0', '1.5', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('15', '16', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('16', '18', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('17', '50', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('18', '19', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('19', '20', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('20', '22', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('21', '21', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('22', '46', '1.2017', '1', '0', '1.5', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('23', '17', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('24', '24', '1.2017', '1.5', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('25', '25', '1.2017', '1.5', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('26', '26', '1.2017', '1.5', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('27', '27', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('28', '15', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('29', '39', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('30', '31', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('31', '32', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('32', '33', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('33', '34', '1.2017', '1', '2', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('34', '45', '1.2017', '0', '0', '0', '0');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('35', '47', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('36', '29', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('37', '37', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('38', '28', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('39', '36', '1.2017', '1', '0', '0', '2');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('40', '2', '1.2017', '1', '0', '0', '1');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('41', '8', '1.2017', '0', '0', '0', '0');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('42', '23', '1.2017', '0', '0', '0', '0');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('43', '35', '1.2017', '0', '0', '0', '0');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('44', '38', '1.2017', '0', '0', '0', '0');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('45', '40', '1.2017', '0', '0', '0', '0');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('46', '41', '1.2017', '0', '0', '0', '0');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('47', '43', '1.2017', '0', '0', '0', '0');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('48', '48', '1.2017', '0', '0', '0', '0');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('49', '49', '1.2017', '0', '0', '0', '0');");
    query.exec("INSERT INTO unita_reperibilita (\"id\", \"id_unita\", \"data\", \"feriale\", \"sabato\", \"prefestivo\", \"festivo\") VALUES ('50', '30', '1.2017', '1.5', '0', '0', '2.5');");
}

void SqlQueries::insertUnit(const QString &raggruppamento,
                            const QString &nome_full,
                            const QString &nome_mini,
                            const QString &numero,
                            const QString &altri_nomi)
{
    QSqlQuery query;
    query.prepare("INSERT INTO unita (raggruppamento,nome_full,nome_mini,numero,altri_nomi) "
                  "VALUES (:raggruppamento, :nome_full, :nome_mini, :numero, :altri_nomi);");
    query.bindValue(":raggruppamento", raggruppamento);
    query.bindValue(":nome_full", nome_full);
    query.bindValue(":nome_mini", nome_mini);
    query.bindValue(":numero", numero);
    query.bindValue(":altri_nomi", altri_nomi);

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::editUnit(const QString &id,
                          const QString &raggruppamento,
                          const QString &nome_full,
                          const QString &nome_mini,
                          const QString &numero,
                          const QString &altri_nomi)
{
    QSqlQuery query;
    query.prepare("UPDATE unita "
                  "SET raggruppamento=:raggruppamento,nome_full=:nome_full,nome_mini=:nome_mini,numero=:numero,altri_nomi=:altri_nomi "
                  "WHERE id=" + id + ";");
    query.bindValue(":raggruppamento", raggruppamento);
    query.bindValue(":nome_full", nome_full);
    query.bindValue(":nome_mini", nome_mini);
    query.bindValue(":numero", numero);
    query.bindValue(":altri_nomi", altri_nomi);

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

bool SqlQueries::insertDoctor(const QString &matricola,
                              const QString &nome,
                              const QString &id_unita)
{
    QSqlQuery query;
    query.prepare("INSERT INTO medici (matricola,nome,id_unita  ) "
                  "VALUES (:matricola, :nome, :id_unita);");
    query.bindValue(":matricola", matricola);
    query.bindValue(":nome", nome);
    query.bindValue(":id_unita", id_unita);

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return false;
    }
    return true;
}

void SqlQueries::editDoctor(const QString &id,
                            const QString &matricola,
                            const QString &nome,
                            const QString &id_unita)
{
    QSqlQuery query;
    query.prepare("UPDATE medici "
                  "SET matricola=:matricola,nome=:nome,id_unita=:id_unita "
                  "WHERE id=" + id + ";");
    query.bindValue(":matricola", matricola);
    query.bindValue(":nome", nome);
    query.bindValue(":id_unita", id_unita);

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::removeUnit(const QString &id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM unita WHERE id=" + id + ";");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::removeDoctor(const QString &id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM medici WHERE id=" + id + ";");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::removeTimeCard(const QString &tableName, const QString &doctorId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM " + tableName + " WHERE id_medico=" + doctorId + ";");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

bool SqlQueries::createTimeCardsTable(const QString &tableName)
{
    QSqlQuery query;
    query.prepare("CREATE TABLE " + tableName + " "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                  "id_medico INTEGER NOT NULL,"
                  "id_unita INTEGER NOT NULL,"
                  "anno INTEGER NOT NULL,"
                  "mese INTEGER NOT NULL,"
                  "riposi INTEGER NOT NULL,"
                  "minuti_giornalieri INTEGER NOT NULL,"
                  "ferie TEXT DEFAULT '',"
                  "congedi TEXT DEFAULT '',"
                  "malattia TEXT DEFAULT '',"
                  "rmp TEXT DEFAULT '',"
                  "rmc TEXT DEFAULT '',"
                  "altre_assenze TEXT DEFAULT '',"
                  "guardie_diurne TEXT DEFAULT '',"
                  "guardie_notturne TEXT DEFAULT '',"
                  "grep TEXT DEFAULT '',"
                  "congedi_minuti INTEGER DEFAULT (0),"
                  "eccr_minuti INTEGER DEFAULT (0),"
                  "grep_minuti INTEGER DEFAULT (0),"
                  "guar_minuti INTEGER DEFAULT (0),"
                  "rmc_minuti INTEGER DEFAULT (0),"
                  "minuti_fatti INTEGER DEFAULT (0));");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return false;
    }

    QString modTableName = tableName;

    query.prepare("CREATE TABLE " + modTableName.replace("_","m_") + " "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                  "id_medico INTEGER NOT NULL,"
                  "guardie_diurne TEXT DEFAULT '',"
                  "guardie_notturne TEXT DEFAULT '',"
                  "turni_reperibilita TEXT DEFAULT '',"
                  "dmp INTEGER DEFAULT (0),"
                  "altre_assenze TEXT DEFAULT '');");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return false;
    }

    return true;
}

bool SqlQueries::tableExists(const QString &tableName)
{
    int count = 0;
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='" + tableName + "';");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        count = query.value(0).toInt();
    }

    if(count <= 0)
        return false;

    return true;
}

bool SqlQueries::timeCardExists(const QString &tableName, const QString &doctorId)
{
    int count = 0;
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM " + tableName + " WHERE id_medico='" + doctorId + "';");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        count = query.value(0).toInt();
    }

    if(count <= 0)
        return false;

    return true;
}

bool SqlQueries::addTimeCard(const QString &tableName, const Dipendente *dipendente)
{
    QSqlQuery query;

    int docId = doctorId(dipendente->matricola());
    int unId = unitId(dipendente->unita());

    if(unId == -1) {
        qDebug() << "Impossibile trovare l'unità" << dipendente->unita();
        return false;
    }

    if(docId == -1) {
//        qDebug() << "---> INSERISCO" << dipendente->matricola() << dipendente->nome() << QString::number(unId);
        if(!insertDoctor(dipendente->matricola(),dipendente->nome(),QString::number(unId)))
            return false;
    }

    docId = doctorId(dipendente->matricola());

    if(timeCardExists(tableName, QString::number(docId))) {
//        qDebug() << "---> RIMUOVO " << dipendente->nome() << "da" << tableName;
        removeTimeCard(tableName, QString::number(docId));
    }

    query.prepare("INSERT INTO " + tableName + " (id_medico, id_unita, anno, mese, riposi, minuti_giornalieri, ferie, congedi, malattia, rmp, rmc, altre_assenze, guardie_diurne, guardie_notturne, grep, congedi_minuti, eccr_minuti, grep_minuti, guar_minuti, rmc_minuti, minuti_fatti) "
                  "VALUES (:id_medico, :id_unita, :anno, :mese, :riposi, :minuti_giornalieri, :ferie, :congedi, :malattia, :rmp, :rmc, :altre_assenze, :guardie_diurne, :guardie_notturne, :grep, :congedi_minuti, :eccr_minuti, :grep_minuti, :guar_minuti, :rmc_minuti, :minuti_fatti);");
    query.bindValue(":id_medico", QString::number(docId));
    query.bindValue(":id_unita", unId);
    query.bindValue(":anno", dipendente->anno());
    query.bindValue(":mese", dipendente->mese());
    query.bindValue(":riposi", dipendente->riposi());
    query.bindValue(":minuti_giornalieri", dipendente->minutiGiornalieri());
    query.bindValue(":ferie", dipendente->ferie().join(","));
    query.bindValue(":congedi", dipendente->congedi().join(","));
    query.bindValue(":malattia", dipendente->malattia().join(","));
    query.bindValue(":rmp", dipendente->rmp().join(","));
    query.bindValue(":rmc", dipendente->rmc().join(","));

    QString altreAssenze = "";
    QMap<QString, QPair<QStringList, int> >::const_iterator i = dipendente->altreAssenze().constBegin();
    while (i != dipendente->altreAssenze().constEnd()) {
        altreAssenze += i.key() + "," + i.value().first.join("~") + "," + QString::number(i.value().second) + ";";
        ++i;
    }
    query.bindValue(":altre_assenze", altreAssenze);

    query.bindValue(":guardie_diurne", dipendente->guardieDiurne().join(","));
    query.bindValue(":guardie_notturne", dipendente->guardieNotturne().join(","));

    QString grep = "";
    QMap<int, QPair<int, int>>::const_iterator it = dipendente->grep().constBegin();
    while (it != dipendente->grep().constEnd()) {
        grep += QString::number(it.key()) + "," + QString::number(it.value().first) + "," + QString::number(it.value().second) + ";";
        ++it;
    }
    query.bindValue(":grep", grep);

    query.bindValue(":congedi_minuti", dipendente->minutiCongedi());
    query.bindValue(":eccr_minuti", dipendente->minutiEccr());
    query.bindValue(":grep_minuti", dipendente->minutiGrep());
    query.bindValue(":guar_minuti", dipendente->minutiGuar());
    query.bindValue(":rmc_minuti", dipendente->minutiRmc());
    query.bindValue(":minuti_fatti", dipendente->minutiFatti());

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return false;
    }

    QString modTableName = tableName;
    modTableName.replace("_","m_");

    query.prepare("INSERT INTO " + modTableName + " (id_medico) "
                  "VALUES (:id_medico);");
    query.bindValue(":id_medico", QString::number(docId));
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return false;
    }

    return true;
}

int SqlQueries::doctorId(const QString &matricola)
{
    int id = -1;
    QSqlQuery query;
    query.prepare("SELECT id FROM medici WHERE matricola='" + matricola + "';");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        id = query.value(0).toInt();
    }
    return id;
}

int SqlQueries::unitId(const QString &unit)
{
    int id = -1;
//    QSqlQuery query;
//    query.prepare("SELECT id FROM unita WHERE altri_nomi LIKE '%" + QString(unit).replace("'", "''") + "%';");
//    if(!query.exec()) {
//        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
//    }
//    while(query.next()) {
//        id = query.value(0).toInt();
//    }
    QMap<int, QStringList>::const_iterator i = m_unitsMap.constBegin();
    while (i != m_unitsMap.constEnd()) {
        if(i.value().contains(unit)) {
            id = i.key();
            break;
        }
        ++i;
    }
    return id;
}

void SqlQueries::buildUnitsMap()
{
    m_unitsMap.clear();
    QSqlQuery query;
    query.prepare("SELECT id,altri_nomi FROM unita;");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }

    while(query.next()) {
        m_unitsMap[query.value(0).toInt()] = query.value(1).toString().split("~");
    }
}
