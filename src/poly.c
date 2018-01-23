#include "poly.h"

int CompareExp(const void *m, const void *n) {

    if ((((Mono *) m)->exp) > (((Mono *) n)->exp)) {
        return -1;
    }

    if ((((Mono *) m)->exp) == (((Mono *) n)->exp)) {
        return 0;
    }

    return 1;
}

void MulByCoeffCloned(Poly *toMul, poly_coeff_t times) {

    if ( PolyIsCoeff(toMul)) {
        toMul->coeff *= times;
        return;
    }

    Mono *walker = toMul->mono;

    while (walker != NULL) {
        MulByCoeffCloned(&walker->p, times);
        walker = walker->next;
    }
}

Poly MulByCoeff(const Poly *toMul, poly_coeff_t times) {
    if ( times == 0 ) {
        return PolyZero();
    }

    if ( times == 1 ) {
        return PolyClone(toMul);
    }

    if ( PolyIsCoeff(toMul)) {
        return PolyFromCoeff(toMul->coeff * times);
    }
    Poly muled = PolyClone(toMul);

    MulByCoeffCloned(&muled, times);
    if ( !PolyIsCoeff(&muled)) {
        Mono *traveller = muled.mono;
        while (traveller != NULL && traveller->exp == 0) {
            if ( PolyIsCoeff(&traveller->p)) {
                poly_coeff_t tempCoeff = traveller->p.coeff;
                PolyDestroy(&muled);
                return PolyFromCoeff(tempCoeff);
            }
            traveller = traveller->p.mono;
        }
        if ( PolyIsCoeff(&muled.mono->p) && muled.mono->p.coeff == 0 ) {
            return PolyZero();
        }
    }

    return muled;
}


void PolyDestroy(Poly *p) {
    if ( p == NULL) {
        return;
    }

    Mono *temp = p->mono;
    Mono *temp2 = temp;

    while (temp != NULL) {
        temp2 = temp;
        temp = temp->next;
        MonoDestroy(temp2);
        free(temp2);

    }

    p->mono = NULL;
}

Mono *MonoPointerTo(Mono *m) {
    Mono *temp = malloc(sizeof(Mono));

    temp->p = m->p;
    temp->exp = m->exp;
    temp->next = m->next;

    return temp;
}

Poly PolyClone(const Poly *p) {

    if ( PolyIsCoeff(p)) {
        return (Poly) {.coeff = p->coeff, .mono=NULL};
    }

    Mono newMono = MonoClone(p->mono);

    Mono *temp = p->mono->next;
    Mono *temp2 = MonoPointerTo(&newMono);
    Mono *temp3 = temp2;

    while (temp != NULL) {
        newMono = MonoClone(temp);
        temp2->next = MonoPointerTo(&newMono);
        temp = temp->next;
        temp2 = temp2->next;
    }

    return (Poly) {
            .coeff = p->coeff, .mono=temp3
    };
}

int MonoCounter(const Poly *p) {
    Mono *m = p->mono;
    if ( m == NULL) return 1;
    Mono *temp = m->next;
    int i = 1;
    while (temp != NULL) {
        temp = temp->next;
        i++;
    }
    return i;
}


Poly PolyAdd(const Poly *p, const Poly *q) {
    if ( PolyIsCoeff(p) && PolyIsCoeff(q)) {
        return PolyFromCoeff(q->coeff + p->coeff);
    }

    if ( PolyIsZero(p)) {
        return PolyClone(q);
    }
    if ( PolyIsZero(q)) {
        return PolyClone(p);
    }
    if ( p == q ) {
        return MulByCoeff(p, 2);
    }

    Poly Minusq = PolyNeg(q);

    if ( PolyIsEq(p, &Minusq)) {
        PolyDestroy(&Minusq);
        return PolyZero();
    }

    PolyDestroy(&Minusq);

    int j = MonoCounter(p);
    int k = MonoCounter(q);

    Mono monos[j + k];

    Mono *temp = p->mono;

    int i = 0;

    if ( PolyIsCoeff(p)) {
        Mono replace = (Mono) {
                .exp = 0, .p = PolyFromCoeff(p->coeff), .next = NULL
        };
        monos[i] = replace;
        i++;
    } else {
        while (temp != NULL) {
            monos[i] = MonoClone(temp);
            monos[i].next = NULL;
            temp = temp->next;
            i++;
        }
    }

    temp = q->mono;

    if ( PolyIsCoeff(q)) {
        Mono replace = (Mono) {
                .exp=0, .next=NULL, .p=PolyFromCoeff(q->coeff)
        };
        monos[i] = replace;

        i++;
    } else {
        while (temp != NULL) {
            monos[i] = MonoClone(temp);
            monos[i].next = NULL;
            temp = temp->next;
            i++;
        }
    }

    Poly toReturn = PolyAddMonos(j + k, monos);

    if ( !PolyIsCoeff(&toReturn)) {
        Mono *iter = toReturn.mono;
        while (iter != NULL && iter->exp == 0) {
            if ( PolyIsCoeff(&iter->p)) {
                poly_coeff_t tempCoeff = iter->p.coeff;
                PolyDestroy(&toReturn);
                return PolyFromCoeff(tempCoeff);
            }
            iter = iter->p.mono;
        }
    }
    return toReturn;
}

Poly PolyAddMonos(unsigned count, const Mono sortedMonos[]) {

	if (count==1) {
		if (PolyIsZero(&sortedMonos[0].p) || sortedMonos[0].p.coeff==0) {
			return PolyZero();
		}
	}

    qsort((void *) sortedMonos, count, sizeof(Mono), CompareExp);

    if ( sortedMonos[0].exp == 0 ) {
        Poly sum = PolyZero();
        unsigned int i = 0;
		/*lel*/for (; i < count && PolyIsCoeff(&sortedMonos[i].p); i++) {
            sum = PolyFromCoeff(sum.coeff + sortedMonos[i].p.coeff);
        }
        if ( i == count ) {
            return sum;
        } else {
            PolyDestroy(&sum);
        }
    }

    Mono guardian = (Mono) {
            .exp=-1, .p=PolyFromCoeff(-1), .next=NULL
    };

    Poly newPoly = (Poly) {
            .mono = &guardian,
            .coeff = 1
    };

    Mono *traveller = MonoPointerTo(&sortedMonos[0]);
    Mono *saver = &guardian;
    guardian.next = traveller;

    unsigned int i = 0;

    while (i < count - 1) {

        if ( traveller->exp == sortedMonos[i + 1].exp ) {

            Mono beg = (Mono) {
                    .exp=-1, .p=PolyZero(), .next=NULL
            };

            Mono *addList = &beg;

            if ( PolyIsCoeff(&traveller->p)) {
                Mono temp = MonoFromPoly(&traveller->p, 0);
                addList->next = MonoPointerTo(&temp);
                addList = addList->next;
            } else {
                Mono *walker = traveller->p.mono;
                while (walker != NULL) {
                    addList->next = walker;
                    addList = addList->next;
                    walker = walker->next;
                }
            }

            while (i < count - 1 && traveller->exp == sortedMonos[i + 1].exp) {
                if ( PolyIsCoeff(&sortedMonos[i + 1].p)) {
                    Mono temp = MonoFromPoly(&sortedMonos[i + 1].p, 0);
                    addList->next = MonoPointerTo(&temp);
                    addList = addList->next;
                } else {
                    Mono *walker2 = sortedMonos[i + 1].p.mono;
                    while (walker2 != NULL) {
                        addList->next = walker2;
                        addList = addList->next;
                        walker2 = walker2->next;
                    }
                }
                i++;
            }

            addList->next = NULL;

            Mono *walker3 = beg.next;
            int total = 0;
            while (walker3 != NULL) {
                total++;
                walker3 = walker3->next;
            }
            Mono newAdd[total];

            walker3 = beg.next;

            for (int m = 0; m <= total - 1; m++) {
                newAdd[m] = *walker3;
                walker3 = walker3->next;
            }

            Mono redundant = *traveller;
            traveller->p = PolyAddMonos(total, newAdd);

            beg.next = NULL;

            if ( PolyIsZero(&traveller->p)) {
                MonoDestroy(traveller);
                traveller = saver;

            }

        } else {

            Mono *newExp = MonoPointerTo(&sortedMonos[i + 1]);
            saver = traveller;
            traveller->next = newExp;
            traveller = traveller->next;
            i++;
        }
    }

    traveller->next = NULL;
    newPoly.mono = guardian.next;

    if ( PolyIsCoeff(&newPoly)) {
        newPoly.coeff = 0;
    }

//    if (!PolyIsZero(&newPoly) && newPoly.coeff==0) {
//        //polydestroy//
//        return PolyZero();
//    }

    if ( !PolyIsCoeff(&newPoly)) {
        Mono *iter = newPoly.mono;
        while (iter != NULL && iter->exp == 0) {
            if ( PolyIsCoeff(&iter->p)) {
                poly_coeff_t tempCoeff = iter->p.coeff;
                PolyDestroy(&newPoly);
                return PolyFromCoeff(tempCoeff);
            }
            iter = iter->p.mono;
        }
    }
//new

    //new

    return newPoly;
}

Mono *MonoMulAndAllocate(const Mono *m, const Mono *n) {

    Mono muled = (Mono) {
            .exp=m->exp + n->exp, .p=PolyMul(&n->p, &m->p), .next=NULL
    };

    return MonoPointerTo(&muled);

}

Poly PolyMul(const Poly *p, const Poly *q) {

    if ( PolyIsCoeff(p)) {
        return MulByCoeff(q, p->coeff);
    }

    if ( PolyIsCoeff(q)) {
        return MulByCoeff(p, q->coeff);
    }

    Mono *tempp = p->mono;
    Mono *tempq = q->mono;

    Mono guardian = (Mono) {
            .exp=-1, .p=PolyZero(), .next=NULL
    };

    Poly mulList = (Poly) {
            .coeff=1, .mono=&guardian
    };

    Poly toAdd = PolyZero();

    Mono *traveller = &guardian;

    while (tempp != NULL) {
        tempq = q->mono;
        guardian = (Mono) {
                .exp=-1, .p=PolyZero(), .next=NULL
        };

        mulList = (Poly) {
                .coeff=1, .mono=&guardian
        };
        traveller = &guardian;

        while (tempq != NULL) {
            traveller->next = MonoMulAndAllocate(tempq, tempp);
            traveller = traveller->next;
            tempq = tempq->next;
        }
        mulList.mono = guardian.next;
        Poly toDestroy = toAdd;
        toAdd = PolyAdd(&toAdd, &mulList);
        tempp = tempp->next;

        PolyDestroy(&toDestroy);
        MonoDestroy(&guardian);
        PolyDestroy(&mulList);
    }
    return toAdd;
}

poly_coeff_t Exp(poly_coeff_t base, poly_exp_t power) {
    poly_coeff_t result = 1;
    while (power > 0) {
        if ( power % 2 == 1 ) {
            result = (result * base);
        }
        base = (base * base);
        power = power / 2;
    }
    return result;
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {

    if ( PolyIsCoeff(p)) {
        return *p;
    }

    Poly polyAt = PolyZero();

    Mono *temp = p->mono;

    while (temp != NULL) {
        Poly muled = MulByCoeff(&temp->p, Exp(x, temp->exp));

        Poly newsum = PolyAdd(&polyAt, &muled);

        PolyDestroy(&polyAt);
        PolyDestroy(&muled);

        polyAt = newsum;
        temp = temp->next;
    }
    return polyAt;
}


bool PolyIsEq(const Poly *p, const Poly *q) {

    if ( PolyIsCoeff(p) && PolyIsCoeff(q) && !(p->coeff == q->coeff)) {
        return false;
    }
    if ( PolyIsCoeff(p) && !PolyIsCoeff(q)) {
        return false;
    }
    if ( !PolyIsCoeff(p) && PolyIsCoeff(q)) {
        return false;
    }

    if ( p == NULL && q == NULL) {
        return true;
    }
    if ( p->coeff != q->coeff ) return false;

    return MonoIsEq(p->mono, q->mono);

}

bool MonoIsEq(const Mono *m, const Mono *n) {
    if ( m == NULL && n == NULL) return true;

    else if ( m == NULL || n == NULL) return false;
    else if ( m->exp != n->exp ) return false;

    if ( !PolyIsEq(&m->p, &n->p)) return false;
    return MonoIsEq(m->next, n->next);
}

poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx) {
    if ( p->coeff == 0 ) return -1;
    if ( PolyIsCoeff(p)) return 0;

    if ( var_idx == 0 ) return p->mono->exp;

    Mono *temp = p->mono;
    poly_exp_t tempint;
    poly_exp_t currentMax = 0;
    while (temp != NULL) {
        tempint = PolyDegBy(&temp->p, var_idx - 1);
        if ( tempint > currentMax ) {
            currentMax = tempint;
        }
        temp = temp->next;
    }
    return currentMax;
}


poly_exp_t PolyDeg(const Poly *p) {
    if ( PolyIsZero(p)) {
        return -1;
    }
    if ( PolyIsCoeff(p)) {
        return 0;
    }

    Mono *temp = p->mono;
    poly_exp_t currentMax = 0;
    poly_exp_t tempint = 0;

    while (temp != NULL) {
        tempint = PolyDeg(&temp->p) + temp->exp;

        if ( currentMax < tempint ) {
            currentMax = tempint;

        }
        temp = temp->next;
    }
    return currentMax;
}
