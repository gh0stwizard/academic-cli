/*
    academic-cli -- a command line utility for ACADEMIC website.
    Copyright (C) 2017  Vitaliy V. Tokarev

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ACADEMIC_H__
#define ACADEMIC_H__

/* seek4term.php logic */
#define ACADEMIC_URL_TERM_FMT "http://academic.ru/seek4term.php" \
                              "?json=true&limit=%d&did=%s&q=%s"

#ifndef ACADEMIC_TERM_LIMIT
#define ACADEMIC_TERM_LIMIT 20
#endif

typedef enum {
	/* en -> ru */
	ACADEMIC_DID_UNIVERSAL_EN_RU,
	ACADEMIC_DID_ENG_RUS,
	ACADEMIC_DID_ENG_RUS_APRESYAN,
	ACADEMIC_DID_BIG_EN_RU,
	ACADEMIC_DID_COMBINED_EN_RU,
	ACADEMIC_DID_MOSTITSKY_EN_RU,
	ACADEMIC_DID_MULLER,
	ACADEMIC_DID_EFIN_DIC,
	ACADEMIC_DID_FINANCE_AND_DEBT,
	ACADEMIC_DID_ENG_RUS_ECONOMY,
	ACADEMIC_DID_ECONOMY_EN_RU,
	ACADEMIC_DID_DEPOSITORY_EN_RU,
	ACADEMIC_DID_LAW_EN_RU,
	ACADEMIC_DID_POLITICS_EN_RU,
	ACADEMIC_DID_INSURANCE_EN_RU,
	ACADEMIC_DID_MATHEMATICS_STATISTICS_EN_RU,
	ACADEMIC_DID_POLYTECHNIC_EN_RU,
	ACADEMIC_DID_AUTO_EN_RU,
	ACADEMIC_DID_PHYSICS_EN_RU,
	ACADEMIC_DID_PLASTICS_EN_RU,
	ACADEMIC_DID_TECHNICAL_EN_RU,
	ACADEMIC_DID_COMPUTER_SECURITY_EN_RU,
	ACADEMIC_DID_MECHANICAL_ENGINEERING_EN_RU,
	ACADEMIC_DID_CIVIL_AVIATION_EN_RU,
	ACADEMIC_DID_AIRCRAFT_SPACE_MATERIALS,
	ACADEMIC_DID_TECHNICAL_ABBREVIATIONS,
	ACADEMIC_DID_EN_RU_METALLURGY,
	ACADEMIC_DID_ROLLING_EN_RU,
	ACADEMIC_DID_ALUMINIUM_EN_RU,
	ACADEMIC_DID_EN_RU_GEOLOGY,
	ACADEMIC_DID_HYDROGEOLOGY_EN_RU,
	ACADEMIC_DID_EN_RU_GEOECOLOGICAL,
	ACADEMIC_DID_OIL_EN_RU,
	ACADEMIC_DID_ENG_RUS_BUILDING,
	ACADEMIC_DID_TRAFFIC_EN_RU,
	ACADEMIC_DID_ECOLOGY_EN_RU,
	ACADEMIC_DID_BIOLOGY_EN_RU,
	ACADEMIC_DID_BIOTECHNOLOGY_EN_RU,
	ACADEMIC_DID_PLANT_TISSUE_CULTURE_EN_RU,
	ACADEMIC_DID_GENETIC_TERMS_EN_RU,
	ACADEMIC_DID_MEDICINE_EN_RU,
	ACADEMIC_DID_IMMUNOLOGY_EN_RU,
	ACADEMIC_DID_WINE_EN_RU,
	ACADEMIC_DID_EN_RU_OPHTHALMOLOGY,
	ACADEMIC_DID_SOCIOLOGY_EN_RU,
	ACADEMIC_DID_APHORISMS_EN_RU,
	ACADEMIC_DID_MID_EN_RU,
	ACADEMIC_DID_ENGLISH_PARTICLES,
	ACADEMIC_DID_SYNONYMUM_EN_RU,
	/* ru -> en*/
	ACADEMIC_DID_SYNONYMUM_RU_EN,
	/* ru: biology */
	ACADEMIC_DID_ENC_BIOLOGY,
	/* must be latest */
	ACADEMIC_DID_MAX
} academic_did;

extern const char *academic_durl_fmt[ACADEMIC_DID_MAX];
extern const char *academic_did_name[ACADEMIC_DID_MAX];

extern const char *academic_dname_en[ACADEMIC_DID_MAX];
extern const char *academic_dname_ru[ACADEMIC_DID_MAX];

#endif /* ACADEMIC_H__ */
