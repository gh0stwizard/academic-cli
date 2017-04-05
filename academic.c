#include "academic.h"


const char *academic_durl_fmt[ACADEMIC_DID_MAX] = {
	/* en -> ru */
	"http://universal_en_ru.academic.ru/%d",
	"http://dic.academic.ru/dic.nsf/eng_rus/%d",
	"http://dic.academic.ru/dic.nsf/eng_rus_apresyan/%d",
	"http://synonymum_en_ru.academic.ru/%d",
	/* ru -> en */
	"http://synonymum_ru_en.academic.ru/%d",
	/* ru: biology */
	"http://dic.academic.ru/dic.nsf/enc_biology/%d"
};

const char *academic_did_name[ACADEMIC_DID_MAX] = {
	/* en -> ru */
	"universal_en_ru",
	"eng_rus",
	"eng_rus_apresyan",
	"synonymum_en_ru",
	/* ru -> en */
	"synonymum_ru_en",
	/* ru: biology */
	"enc_biology"
};

const char *academic_did_desc_en[ACADEMIC_DID_MAX] = {
	/* en -> ru */
	"Universal English-Russian Dictionary",
	"Big English-Russian & Russian-English Dictionary",
	"New Big English-Russian Dictionary",
	"English-Russian Synonyms Dictionary",
	/* ru -> en */
	"Russian-English Synonyms Dictionary",
	/* ru: biology */
	"Biological Encyclopedia",
};

const char *academic_did_desc_ru[ACADEMIC_DID_MAX] = {
	/* en -> ru */
	"Универсальный англо-русский словарь",
	"Большой англо-русский и русско-английский словарь",
	"Новый большой англо-русский словарь",
	"Англо-русский синонимический словарь",
	/* ru -> en */
	"Русско-английский синонимический словарь",
	/* ru: biology */
	"Биологическая энциклопедия"
};
