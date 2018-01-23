/** @file
   Interfejs klasy wielomianA3w

   @author Jakub Pawlewicz <pan@mimuw.edu.pl>,
   @copyright Uniwersytet Warszawski
   @date 2017-04-24,
*/


#ifndef __POLY_H__
#define __POLY_H__

#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/** Typ wspólczynników wielomianu */
typedef long poly_coeff_t;

/** Typ wykladników wielomianu */
typedef int poly_exp_t;

/**
 * Struktura przechowujaca wielomian
 *
 */

typedef struct Poly {
	struct Mono* mono;///<jednomian
	poly_coeff_t coeff;///<wspolczynnik

} Poly;

/**
  * Struktura przechowujaca jednomian
  * Jednomian ma postac `p * x^e`.
  * Wspólczynnik `p` mozee tez byc wielomianem.
  * Bedzie on traktowany jako wielomian nad kolejna zmienna (nie nad x).
  */
typedef struct Mono {
	Poly p; ///< wspólczynnik
	poly_exp_t exp; ///< wykladnik
	struct Mono* next; ///<nastepny mono
} Mono;

/**
 * Tworzy wielomian, który jest wspólczynnikiem.
 * @param[in] c : wartosc wspólczynnika
 * @return wielomian
 */
static inline Poly PolyFromCoeff(poly_coeff_t c) {

	return (Poly) {
			.coeff = c, .mono=NULL
	};
}



/**
 * Tworzy wielomian tozsamosciowo równy zeru.
 * @return wielomian
 */
static inline Poly PolyZero() {
	return PolyFromCoeff(0);
}

/**
 * Tworzy jednomian `p * x^e`.
 * Tworzony jednomian przejmuje na wlasnosc (kopiuje) wielomian @p p.
 * @param[in] p : wielomian - wspólczynnik jednomianu
 * @param[in] e : wykladnik
 * @return jednomian `p * x^e`
 */
static inline Mono MonoFromPoly(const Poly *p, poly_exp_t e) {
	return (Mono) {
			.p = *p, .exp = e, .next=NULL
	};
}

/**
 * Sprawdza, czy wielomian jest wspólczynnikiem.
 * @param[in] p : wielomian
 * @return Czy wielomian jest wspólczynnikiem?
 */
static inline bool PolyIsCoeff(const Poly *p) {

	if (p==NULL) return false;
	return p->mono == NULL;
}

/**
 * Sprawdza, czy wielomian jest tozsamosciowo równy zeru.
 * @param[in] p : wielomian
 * @return Czy wielomian jest równy zero?
 */
static inline bool PolyIsZero(const Poly *p) {
	return (p->mono==NULL && p->coeff == 0);
}

/**
 * Usuwa wielomian z pamieci.
 * @param[in] p : wielomian
 */
void PolyDestroy(Poly *p);

/**
 * Usuwa jednomian z pamieci.
 * @param[in] m : jednomian
 */
static inline void MonoDestroy(Mono *m) {
	if (m == NULL) {
		return;
	}

	PolyDestroy(&(m->p));
}

/**
 * Robi pelna kopie wielomianu.
 * @param[in] p : wielomian
 * @return skopiowany wielomian
 */
Poly PolyClone(const Poly *p);

/**
 * Robi pelna kopie jednomianu.
 * @param[in] m : jednomian
 * @return skopiowany jednomian
 */
static inline Mono MonoClone(const Mono *m) {
	return (Mono) {

			.p = PolyClone(&(m->p)), .exp=m->exp, .next=NULL
	};

}

void printMono(Mono *pMono);

void printPoly(Poly *p);

/**
 * Dodaje dwa wielomiany.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p + q`
 */


Poly PolyAdd(const Poly *p, const Poly *q);

/**
 * Sumuje liste jednomianów i tworzy z nich wielomian.
 * @param[in] count : liczba jednomianów
 * @param[in] sortedMonos : tablica jednomianów
 * @return wielomian bedacy suma jednomianów
 */
Poly PolyAddMonos(unsigned count, const Mono sortedMonos[]);

/**
 * Mnozy dwa wielomiany.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p * q`
 */
Poly PolyMul(const Poly *p, const Poly *q);

/**
 * Zwraca przeciwny wielomian.
 * @param[in] p : wielomian
 * @return `-p`
 */
static inline Poly PolyNeg(const Poly *p) {
	Poly temp=PolyFromCoeff(-1);
	return PolyMul(p, &temp);
}

/**
 * Odejmuje wielomian od wielomianu.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p - q`
 */
static inline Poly PolySub(const Poly *p, const Poly *q) {
	Poly temp = PolyNeg(q);
	return PolyAdd(p, &temp);
}

/**
 * Zwraca stopien wielomianu ze wzgledu na zadana zmienna (-1 dla wielomianu
 * tozsamosciowo równego zeru).
 * Zmienne indeksowane sa od 0.
 * Zmienna o indeksie 0 oznacza zmienna glowna tego wielomianu.
 * Wieksze indeksy oznaczaja zmienne wielomianów znajdujacych sie
 * we wspólczynnikach.
 * @param[in] p : wielomian
 * @param[in] var_idx : indeks zmiennej
 * @return stopien wielomianu @p p z wzgledu na zmienna o indeksie @p var_idx
 */
poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx);

/**
 * Zwraca stopien wielomianu (-1 dla wielomianu tozsamosciowo równego zeru).
 * @param[in] p : wielomian
 * @return stopien wielomianu @p p
 */
poly_exp_t PolyDeg(const Poly *p);

/**
 * Sprawdza równosc dwóch wielomianów.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p = q`
 */
bool PolyIsEq(const Poly *p, const Poly *q);

/**
 * Sprawdza równosc dwóch jednomianów.
 * @param[in] m : jednomian
 * @param[in] n : jednomian
 * @return `m = n`
 */

bool MonoIsEq(const Mono *m, const Mono *n);

/**
 * Wylicza wartosc wielomianu w punkcie @p x.
 * Wstawia pod pierwsza zmienna wielomianu wartosc @p x.
 * W wyniku moze powstac wielomian, jesli wspólczynniki sa wielomianem
 * i zmniejszane sa indeksy zmiennych w takim wielomianie o jeden.
 * Formalnie dla wielomianu @f$p(x_0, x_1, x_2, \ldots)@f$ wynikiem jest
 * wielomian @f$p(x, x_0, x_1, \ldots)@f$.
 * @param[in] p
 * @param[in] x
 * @return @f$p(x, x_0, x_1, \ldots)@f$
 */
Poly PolyAt(const Poly *p, poly_coeff_t x);

/**
* Bierze jednomian i daje wskaznik na niego.
* Pomocne w przypadku wielokrotnego zajmowania pamieci.
* @param[in] m
* @return wskaznik na m
*/

Mono* MonoPointerTo(Mono* m);

/**
* Bierze coefficient wielomianu i potege do którego
* ma go podniesc i to robi.
* @param[in] base
* @param[in] power
*
*/

poly_coeff_t Exp(poly_coeff_t base, poly_exp_t power);

/**
* Funkcja zliczajaca jednomiany wielomianu.
* @param[in] p
* @return ilosc jednomianów @p p
*/

int MonoCounter(const Poly *p);

/**
* Funkcja pomocnicza do qsort ze standardowej biblioteki.
* @param[in] m
* @param[in] n
* @return 1 jesli exp n > exp m, 0 gdy =, -1 gdy <
*/

int CompareExp(const void * m, const void * n);

/**
* Funkcja mnozacaca wielomian przez coeff.
* Przydatna do np. funkcji mnozacej.
* @param[in] toMul
* @param[in] times
* @return toMul * times
*/

Poly MulByCoeff (const Poly* toMul, poly_coeff_t times);

/**
* Funkcja wspierajaca do funkcji mnozacej przez coeff.
* Mnozy juz sklonowany wielomian.
* @param[in] toMul
* @param[in] times
* @return toMul * times
*/

void MulByCoeffCloned (Poly* toMul, poly_coeff_t times);


#endif /* __POLY_H__ */
