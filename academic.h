#ifndef ACADEMIC_H__
#define ACADEMIC_H__

/* seek4term.php logic */
#define ACADEMIC_URL_TERM_FMT "http://academic.ru/seek4term.php" \
                              "?json=true&limit=%d&did=%s&q=%s"

#ifndef ACADEMIC_TERM_LIMIT
#define ACADEMIC_TERM_LIMIT 20
#endif

typedef enum {
	/**
	 * English -> Russian
	 */
	/* Универсальный англо-русский словарь */
	ACADEMIC_DID_UNIVERSAL_EN_RU,
	/* Большой англо-русский и русско-английский словарь */
	ACADEMIC_DID_ENG_RUS,
	/* Новый большой англо-русский словарь */
	ACADEMIC_DID_ENG_RUS_APRESYAN,
	/* Англо-русский синонимический словарь */
	ACADEMIC_DID_SYNONYMUM_EN_RU,

	/**
	 * Russian -> English
	 */
	ACADEMIC_DID_SYNONYMUM_RU_EN,

	/**
     * Russian: Biology
	 */
	ACADEMIC_DID_ENC_BIOLOGY,

	/* max limit */
	ACADEMIC_DID_MAX
} academic_did;

extern const char *academic_durl_fmt[ACADEMIC_DID_MAX];
extern const char *academic_did_name[ACADEMIC_DID_MAX];

extern const char *academic_did_desc_en[ACADEMIC_DID_MAX];
extern const char *academic_did_desc_ru[ACADEMIC_DID_MAX];

#endif /* ACADEMIC_H__ */
