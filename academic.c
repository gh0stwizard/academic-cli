#include "academic.h"


const char *academic_durl_fmt[ACADEMIC_DID_MAX] = {
	/* en -> ru */
	"http://universal_en_ru.academic.ru/%d",
	"http://dic.academic.ru/dic.nsf/eng_rus/%d",
	"http://dic.academic.ru/dic.nsf/eng_rus_apresyan/%d",
	"http://synonymum_en_ru.academic.ru/%d",
	/* ru -> en */
	"http://synonymum_ru_en.academic.ru/%d"
};

const char *academic_did_name[ACADEMIC_DID_MAX] = {
	/* en -> ru */
	"universal_en_ru",
	"eng_rus",
	"eng_rus_apresyan",
	"synonymum_en_ru",
	/* ru -> en */
	"synonymum_ru_en"
};
