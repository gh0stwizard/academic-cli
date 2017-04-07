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

#include "academic.h"


const char *academic_durl_fmt[ACADEMIC_DID_MAX] = {
	/* en -> ru */
	"http://universal_en_ru.academic.ru/%d",
	"http://dic.academic.ru/dic.nsf/eng_rus/%d",
	"http://dic.academic.ru/dic.nsf/eng_rus_apresyan/%d",
	"http://big_en_ru.academic.ru/%d",
	"http://combined_en_ru.academic.ru/%d",
	"http://mostitsky_en_ru.academic.ru/%d",
	"http://dic.academic.ru/dic.nsf/muller/%d",
	"http://dic.academic.ru/dic.nsf/efin_dic/%d",
	"http://finance_and_debt.academic.ru/%d",
	"http://dic.academic.ru/dic.nsf/eng_rus_economy/%d",
	"http://economy_en_ru.academic.ru/%d",
	"http://depository_en_ru.academic.ru/%d",
	"http://law_en_ru.academic.ru/%d",
	"http://politics_en_ru.academic.ru/%d",
	"http://insurance_en_ru.academic.ru/%d",
	"http://mathematics_statistics_en_ru.academic.ru/%d",
	"http://polytechnic_en_ru.academic.ru/%d",
	"http://auto_en_ru.academic.ru/%d",
	"http://physics_en_ru.enacademic.com/%d",
	"http://plastics_en_ru.academic.ru/%d",
	"http://technical_en_ru.academic.ru/%d",
	"http://computer_security_en_ru.academic.ru/%d",
	"http://mechanical_engineering_en_ru.academic.ru/%d",
	"http://civil_aviation_en_ru.academic.ru/%d",
	"http://aircraft_space_materials.academic.ru/%d",
	"http://technical_abbreviations.academic.ru/%d",
	"http://dic.academic.ru/dic.nsf/en_ru_metallurgy/%d",
	"http://rolling_en_ru.academic.ru/%d",
	"http://aluminium_en_ru.academic.ru/%d",
	"http://dic.academic.ru/dic.nsf/en_ru_geology/%d",
	"http://hydrogeology_en_ru.academic.ru/%d",
	"http://dic.academic.ru/dic.nsf/en_ru_geoecological/%d",
	"http://oil_en_ru.academic.ru/%d",
	"http://eng_rus_building.academic.ru/%d",
	"http://traffic_en_ru.academic.ru/%d",
	"http://ecology_en_ru.academic.ru/%d",
	"http://biology_en_ru.academic.ru/%d",
	"http://biotechnology_en_ru.academic.ru/%d",
	"http://plant_tissue_culture_en_ru.academic.ru/%d",
	"http://genetic_terms_en_ru.academic.ru/%d",
	"http://medicine_en_ru.academic.ru/%d",
	"http://immunology_en_ru.academic.ru/%d",
	"http://wine_en_ru.academic.ru/%d",
	"http://dic.academic.ru/dic.nsf/en_ru_ophthalmology/%d",
	"http://sociology_en_ru.academic.ru/%d",
	"http://aphorisms_en_ru.academic.ru/%d",
	"http://mid_en_ru.academic.ru/%d",
	"http://english_particles.academic.ru/%d",
	"http://synonymum_en_ru.academic.ru/%d",
	/* ru -> en */
	"http://universal_ru_en.academic.ru/%d",
	"http://law_ru_en.academic.ru/%d",
	"http://science_ru_en.academic.ru/%d",
	"http://dic.academic.ru/dic.nsf/rus_eng_mathematics/%d",
	"http://polytechnic_ru_en.enacademic.com/%d",
	"http://auto_ru_en.enacademic.com/%d",
	"http://technical_ru_en.academic.ru/%d",
	"http://mechanical_engineering_ru_en.academic.ru/%d",
	"http://physics_ru_en.enacademic.com/%d",
	"http://dic.academic.ru/dic.nsf/rus_eng_biology/%d",
	"http://biology_ru_en.enacademic.com/%d",
	"http://microbiology_ru_en.academic.ru/%d",
	"http://psychology_ru_en.enacademic.com/%d",
	"http://politics_ru_en.enacademic.com/%d",
	"http://sci_lang_ru_en.academic.ru/%d",
	"http://phraseology_ru_en.academic.ru/%d",
	"http://proverbs_ru_en.academic.ru/%d",
	"http://synonymum_ru_en.academic.ru/%d",
	/* ru: enc: biology */
	"http://dic.academic.ru/dic.nsf/enc_biology/%d"
};

const char *academic_did_name[ACADEMIC_DID_MAX] = {
	/* en -> ru */
	"universal_en_ru",
	"eng_rus",
	"eng_rus_apresyan",
	"big_en_ru",
	"combined_en_ru",
	"mostitsky_en_ru",
	"muller",
	"efin_dic",
	"finance_and_debt",
	"eng_rus_economy",
	"economy_en_ru",
	"depository_en_ru",
	"law_en_ru",
	"politics_en_ru",
	"insurance_en_ru",
	"mathematics_statistics_en_ru",
	"polytechnic_en_ru",
	"auto_en_ru",
	"physics_en_ru",
	"plastics_en_ru",
	"technical_en_ru",
	"computer_security_en_ru",
	"mechanical_engineering_en_ru",
	"civil_aviation_en_ru",
	"aircraft_space_materials",
	"technical_abbreviations",
	"en_ru_metallurgy",
	"rolling_en_ru",
	"aluminium_en_ru",
	"en_ru_geology",
	"hydrogeology_en_ru",
	"en_ru_geoecological",
	"oil_en_ru",
	"eng_rus_building",
	"traffic_en_ru",
	"ecology_en_ru",
	"biology_en_ru",
	"biotechnology_en_ru",
	"plant_tissue_culture_en_ru",
	"genetic_terms_en_ru",
	"medicine_en_ru",
	"immunology_en_ru",
	"wine_en_ru",
	"en_ru_ophthalmology",
	"sociology_en_ru",
	"aphorisms_en_ru",
	"mid_en_ru",
	"english_particles",
	"synonymum_en_ru",
	/* ru -> en */
	"universal_ru_en",
	"law_ru_en",
	"science_ru_en",
	"rus_eng_mathematics",
	"polytechnic_ru_en",
	"auto_ru_en",
	"technical_ru_en",
	"mechanical_engineering_ru_en",
	"physics_ru_en",
	"rus_eng_biology",
	"biology_ru_en",
	"microbiology_ru_en",
	"psychology_ru_en",
	"politics_ru_en",
	"sci_lang_ru_en",
	"phraseology_ru_en",
	"proverbs_ru_en",	
	"synonymum_ru_en",
	/* ru: enc: biology */
	"enc_biology"
};

const char *academic_dname_en[ACADEMIC_DID_MAX] = {
	/* en -> ru */
	"English-Russian Universal Dictionary",
	"English-Russian & Russian-English Big Dictionary",
	"English-Russian New Big Dictionary",
	"English-Russian Big Universal Translation Dictionary",
	"English-Russian Combined Dictionary",
	"Mostitsky's English-Russian Dictionary",
	"Muller's English-Russian Dictionary",
	"English-Russian Dictionary of Financial Terms",
	"English-Russian Dictionary Finance and Debts",
	"English-Russian Dictionary of Economic Terms",
	"English-Russian Economic Dictionary",
	"English-Russian Dictionary of Terms for Depositary Storage and Clearing",
	"English-Russian Dictionary of Law",
	"English-Russian Politics Dictionary",
	"English-Russian Insurance Dictionary",
	"English-Russian Dictionary of Probability Theory, Statistics and Combinatorics",
	"English-Russian Polytechnical Dictionary",
	"English-Russian Automobile Dictionary",
	"English-Russian Dictionary of Physics",
	"English-Russian Plastics Terminology Dictionary",
	"English-Russian Technical Dictionary",
	"English-Russian Dictionary on Computer Security",
	"English-Russian Dictionary of Mechanical Engineering",
	"English-Russian Dictionary of Civil Aviation",
	"English-Russian Dictionary of Aircraft Space Materials",
	"English-Russian dictionary of Technical Abbreviations",
	"English-Russian Metallurgical Dictionary",
	"English-Russian Dictionary for Rolling Metals",
	"English-Russian Glossary of Aluminum Industry",
	"English-Russian Dictionary of Geology",
	"English-Russian Hydrogeological Dictionary",
	"English-Russian Geoecological Dictionary",
	"English-Russian Dictionary of Oil and Gas Industry",
	"English-Russian Dictionary of Building Terms",
	"English-Russian Road and Transport Dictionary",
	"English-Russian Dictionary of Ecology",
	"English-Russian Biological Dictionary",
	"English-Russian Dictionary of Biotechnology",
	"English-Russian Terminological List of Plant Tissue Culture",
	"English-Russian Explanatory Dictionary of Genetic Terms",
	"English-Russian Medical Dictionary",
	"English-Russian Dictionary of Immunology",
	"English-Russian Dictionary of Wine",
	"English-Russian Ophthalmologic Dictionary",
	"English-Russian Dictionary of Sociology",
	"English-Russian Dictionary of Quotations, Proverbs, Sayings and Idioms",
	"Terminological Dictionary of the Ministry of Foreign Affairs of Russia",
	"English Particles. English-Russian Dictionary",
	"English-Russian Synonyms Dictionary",
	/* ru -> en */
	"Russian-English Universal Dictionary",
	"Russian-English Dictionary of Law",
	"Russian-English Scientific and Technical Dictionary of Interpreter",
	"Russian-English Dictionary of Mathematical Terms",
	"Russian-English Polytechnical Dictionary",
	"Russian-English Automobile Dictionary",
	"Russian-English Technical Dictionary",
	"Russian-English Dictionary of Mechanical Engineering",
	"Russian-English Dictionary of Physics",
	"Russian-English Dictionary of Biological Terms",
	"Russian-English Biological Dictionary",
	"Russian-English Dictionary of Terms for Microbiology",
	"Russian-English Psychology Dictionary",
	"Russian-English Politics Dictionary",
	"Russian-English Dictionary of Scientific Communication",
	"Large Russian-English Phraseological Dictionary",
	"Russian-English Dictionary of Proverbs and Sayings",	
	"Russian-English Synonyms Dictionary",
	/* ru: enc: biology */
	"Russian Biological Encyclopedia",
};

const char *academic_dname_ru[ACADEMIC_DID_MAX] = {
	/* en -> ru */
	"Универсальный англо-русский словарь",
	"Большой англо-русский и русско-английский словарь",
	"Новый большой англо-русский словарь",
	"Англо-русский большой универсальный переводческий словарь",
	"Сборный англо-русский словарь",
	"Англо-русский универсальный дополнительный практический переводческий словарь И. Мостицкого",
	"Англо-русский словарь Мюллера",
	"Англо-русский словарь финансовых терминов",
	"Англо-русский словарь Финансы и долги",
	"Англо-русский словарь экономических терминов",
	"Англо-русский экономический словарь",
	"Англо-русский словарь терминов по депозитарному хранению и клирингу",
	"Англо-русский юридический словарь",
	"Англо-русский словарь политической терминологии",
	"Англо-русский словарь страховых терминов",
	"Англо-русский словарь по теории вероятностей, статистике и комбинаторике",
	"Англо-русский политехнический словарь",
	"Англо-русский автомобильный словарь",
	"Англо-русский физический словарь",
	"Англо-русский словарь по пластикам",
	"Англо-русский технический словарь",
	"Англо-русский словарь по компьютерной безопасности",
	"Англо-русский словарь по машиностроению",
	"Англо-русский словарь по гражданской авиации",
	"Англо-русский словарь по авиационно-космическим материалам",
	"Англо-русский словарь технических аббревиатур",
	"Англо-русский металлургический словарь",
	"Англо-русский словарь по прокатке металлов",
	"Англо-русский глоссарий алюминиевой промышленности",
	"Англо-русский геологический словарь",
	"Англо-русский гидрогеологический словарь",
	"Англо-русский геоэкологический словарь",
	"Англо-русский словарь нефтегазовой промышленности",
	"Англо-русский словарь строительных терминов",
	"Англо-русский дорожно-транспортный словарь",
	"Англо-русский словарь по экологии",
	"Англо-русский словарь биологических терминов",
	"Англо-русский словарь по биотехнологиям",
	"Англо-русский терминологический перечень по культуре тканей растений",
	"Англо-русский толковый словарь генетических терминов",
	"Англо-русский медицинский словарь",
	"Англо-русский словарь по иммунологии",
	"Англо-русский словарь по вину",
	"Англо-русский офтальмологический словарь",
	"Англо-русский словарь по социологии",
	"Англо-русский словарь цитат, пословиц, поговорок и идиом",
	"Терминологический словарь МИД России",
	"Английские частицы. Англо-русский словарь",
	"Англо-русский синонимический словарь",
	/* ru -> en */
	"Универсальный русско-английский словарь",
	"Русско-английский юридический словарь",
	"Русско-английский научно-технический словарь переводчика",
	"Русско-английский словарь математических терминов",
	"Русско-английский политехнический словарь",
	"Русско-английский автомобильный словарь",
	"Русско-английский технический словарь",
	"Русско-английский словарь по машиностроению",
	"Русско-английский словарь по физике",
	"Русско-английский словарь биологических терминов",
	"Русско-английский биологический словарь",
	"Русско-английский словарь терминов по микробиологии",
	"Русско-английский психологический словарь",
	"Русско-английский словарь политической терминологии",
	"Русско-английский словарь научного общения",
	"Большой русско-английский фразеологический словарь",
	"Русско-английский словарь пословиц и поговорок",	
	"Русско-английский синонимический словарь",
	/* ru: enc: biology */
	"Биологическая энциклопедия"
};
