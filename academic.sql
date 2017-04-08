--
-- File generated with SQLiteStudio v3.1.1 on Fri Apr 7 16:57:20 2017
--
-- Text encoding used: UTF-8
--
PRAGMA foreign_keys = off;
BEGIN TRANSACTION;

-- Table: dictionaries
DROP TABLE IF EXISTS dictionaries;
CREATE TABLE dictionaries (id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE, typeid INTEGER REFERENCES dictionary_types (id) NOT NULL, name TEXT (64) NOT NULL, fmt TEXT (1024) NOT NULL);
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (1, 1, 'universal_en_ru', 'http://universal_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (2, 1, 'eng_rus', 'http://dic.academic.ru/dic.nsf/eng_rus/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (3, 1, 'eng_rus_apresyan', 'http://dic.academic.ru/dic.nsf/eng_rus_apresyan/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (4, 1, 'big_en_ru', 'http://big_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (5, 1, 'combined_en_ru', 'http://combined_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (6, 1, 'auto_en_ru', 'http://auto_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (7, 1, 'polytechnic_en_ru', 'http://polytechnic_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (8, 1, 'mathematics_statistics_en_ru', 'http://mathematics_statistics_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (9, 1, 'insurance_en_ru', 'http://insurance_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (10, 1, 'politics_en_ru', 'http://politics_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (11, 1, 'law_en_ru', 'http://law_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (12, 1, 'depository_en_ru', 'http://depository_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (13, 1, 'economy_en_ru', 'http://economy_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (14, 1, 'eng_rus_economy', 'http://dic.academic.ru/dic.nsf/eng_rus_economy/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (15, 1, 'finance_and_debt', 'http://finance_and_debt.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (16, 1, 'efin_dic', 'http://dic.academic.ru/dic.nsf/efin_dic/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (17, 1, 'muller', 'http://dic.academic.ru/dic.nsf/muller/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (18, 1, 'mostitsky_en_ru', 'http://mostitsky_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (19, 1, 'en_ru_geoecological', 'http://dic.academic.ru/dic.nsf/en_ru_geoecological/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (20, 1, 'hydrogeology_en_ru', 'http://hydrogeology_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (21, 1, 'en_ru_geology', 'http://dic.academic.ru/dic.nsf/en_ru_geology/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (22, 1, 'aluminium_en_ru', 'http://aluminium_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (23, 1, 'rolling_en_ru', 'http://rolling_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (24, 1, 'en_ru_metallurgy', 'http://dic.academic.ru/dic.nsf/en_ru_metallurgy/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (25, 1, 'technical_abbreviations', 'http://technical_abbreviations.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (26, 1, 'aircraft_space_materials', 'http://aircraft_space_materials.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (27, 1, 'civil_aviation_en_ru', 'http://civil_aviation_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (28, 1, 'mechanical_engineering_en_ru', 'http://mechanical_engineering_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (29, 1, 'computer_security_en_ru', 'http://computer_security_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (30, 1, 'technical_en_ru', 'http://technical_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (31, 1, 'plastics_en_ru', 'http://plastics_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (32, 1, 'physics_en_ru', 'http://physics_en_ru.enacademic.com/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (33, 1, 'synonymum_en_ru', 'http://synonymum_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (34, 1, 'english_particles', 'http://english_particles.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (35, 1, 'mid_en_ru', 'http://mid_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (36, 1, 'aphorisms_en_ru', 'http://aphorisms_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (37, 1, 'sociology_en_ru', 'http://sociology_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (38, 1, 'en_ru_ophthalmology', 'http://dic.academic.ru/dic.nsf/en_ru_ophthalmology/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (39, 1, 'wine_en_ru', 'http://wine_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (40, 1, 'immunology_en_ru', 'http://immunology_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (41, 1, 'medicine_en_ru', 'http://medicine_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (42, 1, 'genetic_terms_en_ru', 'http://genetic_terms_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (43, 1, 'plant_tissue_culture_en_ru', 'http://plant_tissue_culture_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (44, 1, 'biotechnology_en_ru', 'http://biotechnology_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (45, 1, 'biology_en_ru', 'http://biology_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (46, 1, 'ecology_en_ru', 'http://ecology_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (47, 1, 'traffic_en_ru', 'http://traffic_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (48, 1, 'eng_rus_building', 'http://eng_rus_building.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (49, 1, 'oil_en_ru', 'http://oil_en_ru.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (50, 2, 'synonymum_ru_en', 'http://synonymum_ru_en.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (51, 2, 'proverbs_ru_en', 'http://proverbs_ru_en.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (52, 2, 'phraseology_ru_en', 'http://phraseology_ru_en.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (53, 2, 'sci_lang_ru_en', 'http://sci_lang_ru_en.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (54, 2, 'politics_ru_en', 'http://politics_ru_en.enacademic.com/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (55, 2, 'psychology_ru_en', 'http://psychology_ru_en.enacademic.com/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (56, 2, 'microbiology_ru_en', 'http://microbiology_ru_en.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (57, 2, 'biology_ru_en', 'http://biology_ru_en.enacademic.com/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (58, 2, 'rus_eng_biology', 'http://dic.academic.ru/dic.nsf/rus_eng_biology/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (59, 2, 'physics_ru_en', 'http://physics_ru_en.enacademic.com/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (60, 2, 'mechanical_engineering_ru_en', 'http://mechanical_engineering_ru_en.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (61, 2, 'technical_ru_en', 'http://technical_ru_en.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (62, 2, 'auto_ru_en', 'http://auto_ru_en.enacademic.com/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (63, 2, 'polytechnic_ru_en', 'http://polytechnic_ru_en.enacademic.com/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (64, 2, 'rus_eng_mathematics', 'http://dic.academic.ru/dic.nsf/rus_eng_mathematics/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (65, 2, 'science_ru_en', 'http://science_ru_en.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (66, 2, 'law_ru_en', 'http://law_ru_en.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (67, 2, 'universal_ru_en', 'http://universal_ru_en.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (68, 5, 'chi_rus', 'http://dic.academic.ru/dic.nsf/chi_rus/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (69, 5, 'chinese_russian', 'http://chinese_russian.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (70, 4, 'russian_chinese', 'http://russian_chinese.academic.ru/%d');
INSERT INTO dictionaries (id, typeid, name, fmt) VALUES (71, 3, 'chinese_english', 'http://chinese_english.enacademic.com/%d');

-- Table: dictionary_names
DROP TABLE IF EXISTS dictionary_names;
CREATE TABLE dictionary_names (id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, did INTEGER REFERENCES dictionaries (id) MATCH SIMPLE NOT NULL, langid INTEGER REFERENCES languages (id) MATCH SIMPLE NOT NULL, name TEXT (512) NOT NULL);
INSERT INTO dictionary_names (id, did, langid, name) VALUES (1, 1, 1, 'Универсальный англо-русский словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (2, 1, 2, 'English-Russian Universal Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (3, 17, 2, 'Muller''s English-Russian Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (4, 18, 2, 'Mostitsky''s English-Russian Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (5, 5, 2, 'English-Russian Combined Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (6, 4, 2, 'English-Russian Big Universal Translation Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (7, 3, 2, 'English-Russian New Big Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (8, 2, 2, 'English-Russian & Russian-English Big Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (9, 7, 2, 'English-Russian Polytechnical Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (10, 8, 2, 'English-Russian Dictionary of Probability Theory, Statistics and Combinatorics');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (11, 9, 2, 'English-Russian Insurance Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (12, 10, 2, 'English-Russian Politics Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (13, 11, 2, 'English-Russian Dictionary of Law');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (14, 12, 2, 'English-Russian Dictionary of Terms for Depositary Storage and Clearing');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (15, 13, 2, 'English-Russian Economic Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (16, 14, 2, 'English-Russian Dictionary of Economic Terms');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (17, 15, 2, 'English-Russian Dictionary Finance and Debts');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (18, 16, 2, 'English-Russian Dictionary of Financial Terms');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (19, 24, 2, 'English-Russian Metallurgical Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (20, 25, 2, 'English-Russian dictionary of Technical Abbreviations');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (21, 26, 2, 'English-Russian Dictionary of Aircraft Space Materials');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (22, 27, 2, 'English-Russian Dictionary of Civil Aviation');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (23, 28, 2, 'English-Russian Dictionary of Mechanical Engineering');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (24, 29, 2, 'English-Russian Dictionary on Computer Security');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (25, 30, 2, 'English-Russian Technical Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (26, 31, 2, 'English-Russian Plastics Terminology Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (27, 32, 2, 'English-Russian Dictionary of Physics');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (28, 6, 2, 'English-Russian Automobile Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (29, 45, 2, 'English-Russian Biological Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (30, 46, 2, 'English-Russian Dictionary of Ecology');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (31, 47, 2, 'English-Russian Road and Transport Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (32, 48, 2, 'English-Russian Dictionary of Building Terms');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (33, 49, 2, 'English-Russian Dictionary of Oil and Gas Industry');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (34, 19, 2, 'English-Russian Geoecological Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (35, 20, 2, 'English-Russian Hydrogeological Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (36, 21, 2, 'English-Russian Dictionary of Geology');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (37, 22, 2, 'English-Russian Glossary of Aluminum Industry');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (38, 23, 2, 'English-Russian Dictionary for Rolling Metals');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (39, 33, 2, 'English-Russian Synonyms Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (40, 34, 2, 'English Particles. English-Russian Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (41, 35, 2, 'Terminological Dictionary of the Ministry of Foreign Affairs of Russia');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (42, 36, 2, 'English-Russian Dictionary of Quotations, Proverbs, Sayings and Idioms');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (43, 37, 2, 'English-Russian Dictionary of Sociology');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (44, 38, 2, 'English-Russian Ophthalmologic Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (45, 39, 2, 'English-Russian Dictionary of Wine');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (46, 40, 2, 'English-Russian Dictionary of Immunology');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (47, 41, 2, 'English-Russian Medical Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (48, 42, 2, 'English-Russian Explanatory Dictionary of Genetic Terms');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (49, 43, 2, 'English-Russian Terminological List of Plant Tissue Culture');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (50, 44, 2, 'English-Russian Dictionary of Biotechnology');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (51, 33, 1, 'Англо-русский синонимический словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (52, 34, 1, 'Английские частицы. Англо-русский словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (53, 35, 1, 'Терминологический словарь МИД России');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (54, 36, 1, 'Англо-русский словарь цитат, пословиц, поговорок и идиом');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (55, 37, 1, 'Англо-русский словарь по социологии');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (56, 38, 1, 'Англо-русский офтальмологический словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (57, 39, 1, 'Англо-русский словарь по вину');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (58, 40, 1, 'Англо-русский словарь по иммунологии');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (59, 41, 1, 'Англо-русский медицинский словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (60, 42, 1, 'Англо-русский толковый словарь генетических терминов');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (61, 43, 1, 'Англо-русский терминологический перечень по культуре тканей растений');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (62, 44, 1, 'Англо-русский словарь по биотехнологиям');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (63, 45, 1, 'Англо-русский словарь биологических терминов');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (64, 46, 1, 'Англо-русский словарь по экологии');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (65, 47, 1, 'Англо-русский дорожно-транспортный словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (66, 48, 1, 'Англо-русский словарь строительных терминов');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (67, 49, 1, 'Англо-русский словарь нефтегазовой промышленности');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (68, 19, 1, 'Англо-русский геоэкологический словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (69, 20, 1, 'Англо-русский гидрогеологический словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (70, 21, 1, 'Англо-русский геологический словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (71, 22, 1, 'Англо-русский глоссарий алюминиевой промышленности');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (72, 23, 1, 'Англо-русский словарь по прокатке металлов');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (73, 24, 1, 'Англо-русский металлургический словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (74, 25, 1, 'Англо-русский словарь технических аббревиатур');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (75, 26, 1, 'Англо-русский словарь по авиационно-космическим материалам');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (76, 27, 1, 'Англо-русский словарь по гражданской авиации');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (77, 28, 1, 'Англо-русский словарь по машиностроению');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (78, 29, 1, 'Англо-русский словарь по компьютерной безопасности');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (79, 30, 1, 'Англо-русский технический словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (80, 31, 1, 'Англо-русский словарь по пластикам');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (81, 32, 1, 'Англо-русский физический словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (82, 6, 1, 'Англо-русский автомобильный словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (83, 7, 1, 'Англо-русский политехнический словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (84, 8, 1, 'Англо-русский словарь по теории вероятностей, статистике и комбинаторике');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (85, 9, 1, 'Англо-русский словарь страховых терминов');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (86, 10, 1, 'Англо-русский словарь политической терминологии');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (87, 11, 1, 'Англо-русский юридический словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (88, 12, 1, 'Англо-русский словарь терминов по депозитарному хранению и клирингу');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (89, 13, 1, 'Англо-русский экономический словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (90, 14, 1, 'Англо-русский словарь экономических терминов');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (91, 15, 1, 'Англо-русский словарь Финансы и долги');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (92, 16, 1, 'Англо-русский словарь финансовых терминов');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (93, 17, 1, 'Англо-русский словарь Мюллера');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (94, 18, 1, 'Англо-русский универсальный словарь И. Мостицкого');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (95, 5, 1, 'Сборный англо-русский словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (96, 4, 1, 'Англо-русский большой универсальный переводческий словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (97, 3, 1, 'Новый большой англо-русский словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (98, 2, 1, 'Большой англо-русский и русско-английский словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (99, 50, 1, 'Русско-английский синонимический словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (100, 51, 1, 'Русско-английский словарь пословиц и поговорок');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (101, 52, 1, 'Большой русско-английский фразеологический словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (102, 53, 1, 'Русско-английский словарь научного общения');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (103, 57, 1, 'Русско-английский биологический словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (104, 58, 1, 'Русско-английский словарь биологических терминов');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (105, 59, 1, 'Русско-английский словарь по физике');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (106, 60, 1, 'Русско-английский словарь по машиностроению');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (107, 61, 1, 'Русско-английский технический словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (108, 62, 1, 'Русско-английский автомобильный словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (109, 63, 1, 'Русско-английский политехнический словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (110, 64, 1, 'Русско-английский словарь математических терминов');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (111, 65, 1, 'Русско-английский научно-технический словарь переводчика');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (112, 66, 1, 'Русско-английский юридический словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (113, 67, 1, 'Универсальный русско-английский словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (114, 50, 2, 'Russian-English Synonyms Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (115, 51, 2, 'Russian-English Dictionary of Proverbs and Sayings');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (116, 52, 2, 'Large Russian-English Phraseological Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (117, 53, 2, 'Russian-English Dictionary of Scientific Communication');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (118, 54, 2, 'Russian-English Politics Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (119, 55, 2, 'Russian-English Psychology Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (120, 56, 2, 'Russian-English Dictionary of Terms for Microbiology');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (121, 56, 1, 'Русско-английский словарь терминов по микробиологии');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (122, 55, 1, 'Русско-английский психологический словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (123, 54, 1, 'Русско-английский словарь политической терминологии');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (124, 57, 2, 'Russian-English Biological Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (125, 58, 2, 'Russian-English Dictionary of Biological Terms');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (126, 59, 2, 'Russian-English Dictionary of Physics');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (127, 60, 2, 'Russian-English Dictionary of Mechanical Engineering');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (128, 61, 2, 'Russian-English Technical Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (129, 62, 2, 'Russian-English Automobile Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (130, 63, 2, 'Russian-English Polytechnical Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (131, 64, 2, 'Russian-English Dictionary of Mathematical Terms');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (132, 65, 2, 'Russian-English Scientific and Technical Dictionary of Interpreter');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (133, 66, 2, 'Russian-English Dictionary of Law');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (134, 67, 2, 'Russian-English Universal Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (135, 68, 1, 'Большой китайско-русский словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (136, 68, 2, 'Chinese-Russian Large Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (137, 69, 1, 'Китайско-русский словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (138, 69, 2, 'Chinese-Russian Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (139, 70, 1, 'Русско-китайский словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (140, 70, 2, 'Russian-Chinese Dictionary');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (141, 71, 1, 'Китайско-английский словарь');
INSERT INTO dictionary_names (id, did, langid, name) VALUES (142, 71, 2, 'Chinese-English Dictionary');

-- Table: dictionary_type_desc
DROP TABLE IF EXISTS dictionary_type_desc;
CREATE TABLE dictionary_type_desc (id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, typeid INTEGER REFERENCES dictionary_types (id) MATCH SIMPLE NOT NULL, langid INTEGER REFERENCES languages (id) MATCH SIMPLE NOT NULL, "desc" TEXT (64) NOT NULL);
INSERT INTO dictionary_type_desc (id, typeid, langid, "desc") VALUES (1, 1, 2, 'English-Russian Dictionaries');
INSERT INTO dictionary_type_desc (id, typeid, langid, "desc") VALUES (2, 1, 1, 'Англо-русские словари');
INSERT INTO dictionary_type_desc (id, typeid, langid, "desc") VALUES (3, 2, 2, 'Russian-English Dictionaries');
INSERT INTO dictionary_type_desc (id, typeid, langid, "desc") VALUES (4, 2, 1, 'Русско-английские словари');
INSERT INTO dictionary_type_desc (id, typeid, langid, "desc") VALUES (5, 3, 2, 'Chinese-English Dictionaries');
INSERT INTO dictionary_type_desc (id, typeid, langid, "desc") VALUES (6, 3, 1, 'Китайско-английские словари');
INSERT INTO dictionary_type_desc (id, typeid, langid, "desc") VALUES (7, 4, 1, 'Русско-китайские словари');
INSERT INTO dictionary_type_desc (id, typeid, langid, "desc") VALUES (8, 4, 2, 'Russian-Chinese Dictionaries');
INSERT INTO dictionary_type_desc (id, typeid, langid, "desc") VALUES (9, 5, 1, 'Китайско-русские словари');
INSERT INTO dictionary_type_desc (id, typeid, langid, "desc") VALUES (10, 5, 2, 'Chinese-Russian Dictionaries');

-- Table: dictionary_types
DROP TABLE IF EXISTS dictionary_types;
CREATE TABLE dictionary_types (id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, type TEXT (32) NOT NULL UNIQUE);
INSERT INTO dictionary_types (id, type) VALUES (1, 'en-ru');
INSERT INTO dictionary_types (id, type) VALUES (2, 'ru-en');
INSERT INTO dictionary_types (id, type) VALUES (3, 'ch-en');
INSERT INTO dictionary_types (id, type) VALUES (4, 'ru-ch');
INSERT INTO dictionary_types (id, type) VALUES (5, 'ch-ru');

-- Table: languages
DROP TABLE IF EXISTS languages;
CREATE TABLE languages (id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, lang TEXT (16) UNIQUE NOT NULL);
INSERT INTO languages (id, lang) VALUES (1, 'ru');
INSERT INTO languages (id, lang) VALUES (2, 'en');

-- Index: idx_dictionaries_id
DROP INDEX IF EXISTS idx_dictionaries_id;
CREATE UNIQUE INDEX idx_dictionaries_id ON dictionaries (id);

-- Index: idx_dictionaries_typeid
DROP INDEX IF EXISTS idx_dictionaries_typeid;
CREATE INDEX idx_dictionaries_typeid ON dictionaries (typeid);

-- Index: idx_dictionary_names_did
DROP INDEX IF EXISTS idx_dictionary_names_did;
CREATE INDEX idx_dictionary_names_did ON dictionary_names (did);

-- Index: idx_dictionary_names_id
DROP INDEX IF EXISTS idx_dictionary_names_id;
CREATE UNIQUE INDEX idx_dictionary_names_id ON dictionary_names (id);

-- Index: idx_dictionary_names_langid
DROP INDEX IF EXISTS idx_dictionary_names_langid;
CREATE INDEX idx_dictionary_names_langid ON dictionary_names (langid);

-- Index: idx_dictionary_type_desc_id
DROP INDEX IF EXISTS idx_dictionary_type_desc_id;
CREATE UNIQUE INDEX idx_dictionary_type_desc_id ON dictionary_type_desc (id);

-- Index: idx_dictionary_type_desc_langid
DROP INDEX IF EXISTS idx_dictionary_type_desc_langid;
CREATE INDEX idx_dictionary_type_desc_langid ON dictionary_type_desc (langid);

-- Index: idx_dictionary_type_desc_typeid
DROP INDEX IF EXISTS idx_dictionary_type_desc_typeid;
CREATE INDEX idx_dictionary_type_desc_typeid ON dictionary_type_desc (typeid);

-- Index: idx_dictionary_types_id
DROP INDEX IF EXISTS idx_dictionary_types_id;
CREATE UNIQUE INDEX idx_dictionary_types_id ON dictionary_types (id);

-- Index: idx_dictionary_types_type
DROP INDEX IF EXISTS idx_dictionary_types_type;
CREATE UNIQUE INDEX idx_dictionary_types_type ON dictionary_types (type ASC);

-- Index: idx_languages_id
DROP INDEX IF EXISTS idx_languages_id;
CREATE UNIQUE INDEX idx_languages_id ON languages (id);

-- Index: idx_languages_name
DROP INDEX IF EXISTS idx_languages_name;
CREATE UNIQUE INDEX idx_languages_name ON languages (lang ASC);

-- View: v_dictionary_names
DROP VIEW IF EXISTS v_dictionary_names;
CREATE VIEW v_dictionary_names AS SELECT d.id,
       n.name,
       l.lang,
       t.type
  FROM dictionaries AS d
       INNER JOIN
       dictionary_names AS n ON d.id = n.did
       INNER JOIN
       languages AS l ON n.langid = l.id
       INNER JOIN
       dictionary_types AS t ON d.typeid = t.id;

-- View: v_dictionary_types
DROP VIEW IF EXISTS v_dictionary_types;
CREATE VIEW v_dictionary_types AS SELECT t.type,
       d.[desc],
       l.lang
  FROM dictionary_type_desc AS d
       INNER JOIN
       dictionary_types AS t ON d.typeid = t.id
       INNER JOIN
       languages AS l ON d.langid = l.id
 ORDER BY t.type ASC;

COMMIT TRANSACTION;
PRAGMA foreign_keys = on;
