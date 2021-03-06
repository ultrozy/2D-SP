/* ======================================================================
              TABU SEARCH for d-dimensional BIN PACKING
   ====================================================================== */

/*
 * Based on:
 *
 *     A. Lodi, S. Martello, D. Vigo, 1999.
 *     "Heuristic and Metaheuristic Approaches for a 
 *     Class of Two-Dimensional Bin Packing Problems",
 *     INFORMS Journal of Computing Vol. 11, N. 4, pp 345-357.
 *
 * and
 *
 *     A. Lodi, S. Martello, D. Vigo, 2002.
 *     "Heuristic Algorithms for the Three-Dimensional Bin Packing Problem",
 *     European Journal of Operational Research Vol. 141, N. 2, pp 410-420.
 *
 * INPUT-OUTPUT description in:
 *
 *     A. Lodi, S. Martello, D. Vigo, 2002.
 *     "TSpack: A Unified Tabu Search Code for
 *     Multi-Dimensional Bin Packing Problems",
 *     Technical Report OR/02/3, D.E.I.S. - University of Bologna.
*/              

#include "TSpack.h"

/* SECTION a): core procedures */

/* outer loop of the tabu search algorithm */
int TSpack(
	int d,    int n,   int **w, int *W, int lb, float TL,
	int *ub0, int **x, int *b
    )
{
	int    nCC = 0;
	int    nb, i, j, toReturn;
	int    dv, K, D, t;
	int    nIT = 0;
        float  st, et, tt;

	int    **cw, **cx, *cb, cnb;
	double *ff;

	/* memory allocation of general TS structures */
	kt = (int*)calloc(Kmax, sizeof(int));
	lm = (int*)calloc(Kmax, sizeof(int));
	tl = (double**)calloc(Kmax, sizeof(double*));
	for (i = 0; i < Kmax; i++) {
	    tl[i] = (double*)calloc(Kten, sizeof(double));
	    kt[i] = Kten;
	}

	/* memory allocation */
	cw = (int**)calloc(d, sizeof(int*));
	cx = (int**)calloc(d, sizeof(int*));
	for (i = 0; i < d; i++) {
	    cw[i] = (int*)calloc(n, sizeof(int));
	    cx[i] = (int*)calloc(n, sizeof(int));
	}
	cb = (int*)calloc(n, sizeof(int));
	ff = (double*)calloc(n+1, sizeof(double));

	/* incumbent solution */
	nb = heur(d, n, w, W, x, b);
	*ub0 = nb;
	if ((nb < lb) || (nb < 1)) {
	   toReturn = -1;
	   goto end;
	}
	if (nb == lb) {
	   toReturn = nb;
	   goto end;
	}

	/* initial (trivial) solution */
	cnb = n;
	for (i = 0; i < n; i++) {
	    cb[i] = i; 
            for (j = 0; j < d; j++) {
	        cx[j][i] = 0; 
	        cw[j][i] = w[j][i]; 
	    }
	}

	second(&st);
	/* external loop */
	D = 1; tt = 0.0;
	while ((tt < TL) && (nIT < nITmax)) {
	      dv = 0; K = 1;

	      fllf(d, n, n, cw, cb, cnb, W, ff);
	      t = target(D, ff, cnb);

	      /* internal loop */
	      while (!dv && (nb > lb) && (K < cnb) && (tt < TL) && (nIT < nITmax)) {
	            int Kin = K, _cnb = cnb, flagNewTarget = 0;

	            nIT++;
	            cnb = search(t, &K, &dv, cnb, d, n, cw, W, cx, cb, ff, TL-tt);

	            if (CHECK) {
	               int correct;
	               correct = checkfs(d, n, w, W, x, b);
	               if (!correct) {
	                  printf("\n the intermediate solution is not feasible!\n");
	                  exit(0);
	               }
	            }

	            if (cnb <= 0) {
	               toReturn = -1;
	               printf("\n the intermediate solution is wrong!\n");
	               goto end;
	            }

	            if (cnb < nb) {
	               second(&et); 
	               nCC = 0; 
	               D = K = flagNewTarget = 1;
	               nb = cnb;
	               for (i = 0; i < n; i++) {
	                   b[i] = cb[i];
                           for (j = 0; j < d; j++) x[j][i] = cx[j][i];
	               }
	            }

	            if   ((cnb == _cnb) && (K == Kin)) nCC++; 
	            else nCC = 0;

	            if (nCC == CCmax) { 
	               nCC = 0; 
	               if   (K == Kmax) dv = 1; 
	               else { 
	                  flagNewTarget = 1; 
	                  K++; 
	               } 
	            }

	            if (((K <= Kin) && !dv) || flagNewTarget) { 
	               fllf(d, n, n, cw, cb, cnb, W, ff); 
	               t = target(1, ff, cnb); 
	            }

	            second(&et);
                    tt = et - st;

	      }

	      /* diversification */
	      if ((nb == lb) || (tt > TL) || (nIT == nITmax)) break;
	      else {
	         if ((D < Dmax) && (D < cnb)) { 
	            D++;
	            dv = 0; 
	         } else {
	            int k, tmp, index, *toEmpty;
                    toEmpty = (int*)calloc(n, sizeof(int));
	            index = cnb/2;
	            for (j = 0; j < cnb-1; j++) {
	                if (index > 0) {
	                   tmp = index;
	                   for (i = j+1; i < cnb; i++) if (ff[i] < ff[j]) tmp--;
	                   if (tmp > 0) { 
	                      toEmpty[j] = 1; 
	                      index--; 
	                      continue; 
	                   }
	                }
	                toEmpty[j] = 0;
	            }
	            index = 0;
	            for (j = 0; j < n; j++) {
	                if (toEmpty[cb[j]]) {
	                   if (index < cnb) while (toEmpty[index] == 0) { 
	                                          index++; 
	                                          if (index >= cnb) break; 
	                                    }
	                   cb[j] = index++; 
	                   for (k = 0; k < d; k++) cx[k][j] = 0;
	                }
	            }
                    free(toEmpty);
	            cnb = (index > cnb) ? index : cnb;

	            for (j = 0; j < Kmax; j++) { 
	                lm[j] = 0; 
	                for (i = 0; i < kt[j]; i++) tl[j][i] = 0.0; 
	            }
	            D = 1;
	         }
	      }
	}
        toReturn = nb;

end:
	/* memory de-allocation */
	for (i = 0; i < d; i++) {
            free(cw[i]);
            free(cx[i]);
        }
        free(cw); free(cx); free(cb); free(ff);

	for (i = 0; i < Kmax; i++) free(tl[i]);
	free(tl); free(kt); free(lm);

	return toReturn;
}

/* inner loop of the tabu search algorithm */
int search(
	int   t,  int *K,  int *dv,
	int   nb, int d,   int n,   int **w,
	int   *W, int **x, int *b,  double *ff, float timeLeft
    )
{
	int    next, exit;
	int    item, i, j, k, index, toReturn;
	int    nnb, bK;
	int    sn, snb, tn, tnb, bnb;

	float  st, et;
	double pBest = INFINITY;

	int    **sw, **sx, *sb;
	int    **tw, **tx, *tb;
	int    **bw, **bx, *bb;
	int    *sT, *tT, *kb, *s;

	double *sff;

	/* memory allocation */
	sw = (int**)calloc(d, sizeof(int*));
	sx = (int**)calloc(d, sizeof(int*));
	for (i = 0; i < d; i++) {
	    sw[i] = (int*)calloc(n, sizeof(int));
	    sx[i] = (int*)calloc(n, sizeof(int));
	}
	sb = (int*)calloc(n, sizeof(int));

	tw = (int**)calloc(d, sizeof(int*));
	tx = (int**)calloc(d, sizeof(int*));
	for (i = 0; i < d; i++) {
	    tw[i] = (int*)calloc(n, sizeof(int));
	    tx[i] = (int*)calloc(n, sizeof(int));
	}
	tb = (int*)calloc(n, sizeof(int));

	bw = (int**)calloc(d, sizeof(int*));
	bx = (int**)calloc(d, sizeof(int*));
	for (i = 0; i < d; i++) {
	    bw[i] = (int*)calloc(n, sizeof(int));
	    bx[i] = (int*)calloc(n, sizeof(int));
	}
	bb = (int*)calloc(n, sizeof(int));

	sT = (int*)calloc(n, sizeof(int));
	tT = (int*)calloc(n, sizeof(int));
	kb = (int*)calloc(n+1, sizeof(int));
	s  = (int*)calloc(n, sizeof(int));

	sff = (double*)calloc(n, sizeof(double));

	/* initialization */
	exit = 0; bK = *K - 1; bnb = nnb = nb; 
	second(&st);

	/* external cycle */
	for (item = 0; item < n; item++) {

	    /* select an item in the target bin t */
	    if (b[item] != t) continue;

	    /* determine the next K-tuple of bins */
	    next = nextKT(kb, *K, t, nb);

	    /* internal cycle */
	    while (next && !exit) {
	          double chsn;
	          int _t, i1, i2, _K = *K - 1;

	          /* set S construction */
	          index = 0;
	          for (j = 0; j < n; j++) {
		      if ((j == item) || (kb[b[j]] == 1)) { 
	                 for (k = 0; k < d; k++) sw[k][index] = w[k][j]; 
	                 sT[j] = index++; 
	              } else sT[j] = -1;
	          }
	          sn = index;

	          /* compute A(S) */
	          snb = heur(d, sn, sw, W, sx, sb);
	          if (snb <= 0) { 
                     toReturn = -1;
                     goto end;
                  }

	          fllf(d, n, sn, sw, sb, snb, W, sff);
	          chsn = sff[0]; _t = 0;
	          for (j = 1; j < snb; j++) if (sff[j] < chsn) { 
	                                       chsn = sff[j]; 
	                                       _t = j; 
	                                    }

	          { 
	            int dummy = 0, **wdummy;
	            wdummy = (int**)calloc(d, sizeof(int*));
	            for (i = 0; i< d; i++) wdummy[i] = (int*)calloc(1, sizeof(int));
	            for (i = 0; i< d; i++) wdummy[i][0] = w[i][item];
	            fllf(d, n, 1, wdummy, &dummy, 1, W, ff+nb);
	            for (i = 0; i < d; i++) free(wdummy[i]);
	            free(wdummy);
	          }

	          for (i1 = 0; i1 < snb; i1++)
	            for (i2 = 0; i2 <= nb; i2++)
	              if ((kb[i2]) && (fabs(ff[i2]-sff[i1]) < EPS)) _K--;

	          if (_K < 0) {
	             next = nextKT(kb, *K, t, nb); 
	             continue; 
	          }

	          /* case: A(S) <= K */
	          if (snb <= *K) {
	             double pK = INFINITY;

	             nnb = nb - (*K) + snb;
	             for (j = 0; j < n; j++) if ((b[j] == t) && (j != item)) break;
	             if (j == n) {
	                nnb--; 
	                kb[t]=1; 
	             }

	             if ((snb == *K) && !kb[t]) {
	                pK = getPen(snb, sff, 0, NULL, 1);
	                for (k = 0; k < kt[_K]; k++) {
	                    if (fabs(tl[_K][k] - pK) < EPS) {
	                       next = nextKT(kb, *K, t, nb); 
	                       break; 
	                    }
	                }
	                if (k != kt[_K]) continue; 
	             }

	             index = 0;
	             for (j = 0; j < nb; j++) {
	                 if (kb[j] == 1) s[j] = -1; 
	                 else            s[j] = index++;
	             }
	             for (j = 0; j < n; j++) {
	                 if ((j == item) || (kb[b[j]] == 1)) {
	                    b[j] = index + sb[sT[j]];
	                    for (k = 0; k < d; k++) x[k][j] = sx[k][sT[j]];
	                 } else if (s[b[j]] < 0) {
                                   toReturn = -1;
                                   goto end;
                                } else b[j] = s[b[j]];
	             }

	             if ((snb == *K) && !kb[t]) {
	                tl[_K][lm[_K]] = pK;
	                if (lm[_K] == kt[_K]-1) lm[_K] = 0; 
	                else                    lm[_K] += 1;
	             }

	             /* automatic update of the neighborhood's size */
	             if ((snb < *K) || kb[t]) *K = ((*K > 1) ? (*K-1) : 1);
	             toReturn = nnb;
                     goto end;
	          }
			
	          /* case: A(S) == K+1 */
	          if ((snb == *K + 1) && (*K > 1)) {

	             /* set T construction */
	             index = 0;
	             for (j = 0; j < n; j++) {
	                 if (((b[j] == t)  && (j != item)) || 
	                     ((sT[j] >= 0) && (sb[sT[j]] == _t))) {
	                    for (k = 0; k < d; k++) tw[k][index] = w[k][j]; 
	                    tT[j] = index++;
	                 } else tT[j] = -1;
	             }
	             tn = index;

	             /* compute A(T) */
	             if (tn == 1) {
	                if (tT[item] == 0) {
	                   next = nextKT(kb, *K, t, nb);
	                   continue;
	                } else {
	                   tb[0] = 0;
	                   for (i = 0; i < d; i++) tx[i][0] = 0;
	                   tnb = 1;
	                }
	             } else tnb = heur(d, tn, tw, W, tx, tb);

	             if (tnb == 1) {
	                double pK, *tff;
	                tff = (double*)calloc(tn, sizeof(double));

	                fllf(d, n, tn, tw, tb, tnb, W, tff);
	                pK = getPen(snb, sff, 1, tff, 0);
	                if (pK < pBest) {
	                   for (k = 0; k < kt[_K]; k++) {
	                       if (fabs(tl[_K][k] - pK) < EPS) { 
	                          next = nextKT(kb, *K, t, nb);
	                          break; 
	                       }
	                   }
	                   if (k != kt[_K]) { 
	                      free(tff); 
	                      continue; 
	                   } 

	                   pBest = pK;
	                   bK = _K; kb[t] = 1; 
	                   index = 0;
	                   for (j = 0; j < nb; j++) {
	                       if (kb[j] == 1) s[j] = -1; 
	                       else            s[j] = index++;
	                   }
	                   for (j = 0; j < n; j++) {
	                       if (kb[b[j]] == 1) {
	                          if (tT[j] < 0) {
	                             bb[j] = index + sb[sT[j]];
	                             for (k = 0; k < d; k++) bx[k][j] = sx[k][sT[j]];
	                          } else {
	                             bb[j] = index + _t;
	                             for (k = 0; k < d; k++) bx[k][j] = tx[k][tT[j]];
	                          }
	                       } else {
	                          if (s[b[j]] < 0) {
                                     toReturn = -1; 
	                             free(tff);
                                     goto end;
	                          } else {
	                             bb[j] = s[b[j]];
	                             for (k = 0; k < d; k++) bx[k][j] = x[k][j];
	                          }
	                       }
	                   }
	                }
                        free(tff);
	             } 
	          }
	          second(&et);
	          if (et - st > timeLeft) exit = 1; 
	          else                    next = nextKT(kb, *K, t, nb);
	    }
	    if (exit) break;
	}

	if ((INFINITY - pBest) > EPS) {
	   for (j = 0; j < n; j++) {
 	       b[j] = bb[j];
 	       for (k = 0; k < d; k++) x[k][j] = bx[k][j];
	   }
	   tl[bK][lm[bK]] = pBest;
	   if (lm[bK] == kt[bK]-1) lm[bK] = 0; 
           else                    lm[bK] = lm[bK] + 1;
	} else {
	   if (*K == Kmax) *dv = 1; 
           else            *K = *K + 1;
        }
	toReturn = bnb; 

end:
        /* memory de-allocation */
	for (i = 0; i < d; i++) {
	    free(sw[i]); free(sx[i]);
	    free(tw[i]); free(tx[i]);
	    free(bw[i]); free(bx[i]);
	}
	free(sw); free(sx); free(tw); free(tx); free(bw); free(bx);
	free(sb); free(tb); free(bb);
	free(sT); free(kb); free(s);  free(tT);
	free(sff);

	return toReturn;
}

/* SECTION b): generic selection procedures */

/* compute the heuristic solution through an inner heuristic (whichH) */
int heur(
        int  d, int   n, int **w,
        int *W, int **x, int *b
    )
{
        int   nb = -1;
        int   which;

        which = (d - 2)*100 + whichH;
        switch (which) {
        case 0:                     /* first heuristic for 2d bin packing */
                  nb = HnextFit(n, w, W, x, b, n+1);
                  break;
        case 100:                   /* first heuristic for 3d bin packing */
                  nb = HHnextFit(n, w, W, x, b);
                  break;
        }

        return nb;
}

/* compute a lower bound value through a selected procedure (whichL) */
int lower(
        int d, int n, int **w, int *W
    )
{
        int     i, j, val, lb;
        double  sum1, sum2, value;

        switch(whichL) {
        case 0: /* trivial Lower Bound */
                lb = 1;
                break;
        case 1: /* so-called continous lower bound */
                sum1 = 0;
                for (i = 0; i < n; i++) {
                    val = w[0][i];
                    for (j = 1; j < d; j++) val *= w[j][i];
                    sum1 += val;
                }
                sum2 = W[0];
                for (j = 1; j < d; j++) sum2 *= W[j];
                value = sum1/sum2;
                lb = (int)value;
                if (value - (double)lb > EPS) lb++;
                break;
        }

        return lb;
}

/* compute the filling functions (whichF) */
void fllf(
        int  d, int  n, int nc, int **w, int *b,
        int nb, int *W, double *ff
     )
{
        int     i, j, Size, val;
        double  *A, *c;

        switch (whichF) {
        case 0:
                A = (double*)calloc(nb, sizeof(double));
                c = (double*)calloc(nb, sizeof(double));
                for (i = 0; i < nc; i++) {
                    val = w[0][i];
                    for (j = 1; j < d; j++) val *= w[j][i];
                    A[b[i]] += val;
                    c[b[i]]++;
                }
                Size = W[0];
                for (j = 1; j < d; j++) Size *= W[j];
                for (i = 0; i < nb; i++) ff[i] = ALPHA*A[i]/Size - c[i]/n;
                free(A); free(c);
                break;
        }
}

/* compute the penalty of a move (whichP) */
double getPen(
        int snb, double *sff,
        int tnb, double *tff, int    flag
       )
{
        int    j;
        double aux;
        double pen = 0.0;

        switch (whichP) {
        case 0: /* compute the penalty as minimum */
                if (flag) {
                   pen = sff[0];
                   for (j = 1; j < snb; j++) if (sff[j] < pen) pen = sff[j];
                   return pen;
                }
                if ((snb < 2) || (tnb != 1)) {
                   printf("\n something strange in getPen!");
                   return -1;
                }
                if (sff[0] < sff[1]) {
                   pen = sff[1];
                   aux = sff[0];
                } else {
                   pen = sff[0];
                   aux = sff[1];
                }
                for (j = 2; j < snb; j++)
                 if (sff[j] < aux) {
                    pen = aux;
                    aux = sff[j];
                 } else if (sff[j] < pen) pen = sff[j];
                if (tff[0] < pen) pen = tff[0];
                break;

        case 1: /* compute the penalty as sum */
                if (snb < 1) {
                   printf("\n something strange in getPen!");
                   return -1;
                }
                for (j = 0; j < snb; j++) pen += sff[j];
                break;
        }
        return pen;
}

/* SECTION c): inner heuristic procedures */

/* hybrid next fit heuristic for 2D bin packing */
int HnextFit(
	int  n, int **w, int *W, int **x, int *b, int maxb
    )
{
	int   i, j, ub, hleft, wleft, hcurr;
	int   **pw, **px, *pb, *ord;

	/* memory allocation */
	pw = (int**)calloc(2, sizeof(int*));
	px = (int**)calloc(2, sizeof(int*));
	for (i = 0; i < 2; i++) {
	    pw[i] = (int*)calloc(n, sizeof(int));
	    px[i] = (int*)calloc(n, sizeof(int));
	}
	pb = (int*)calloc(n, sizeof(int));
	for (i = 0; i < n; i++) pb[i] = -1;
	ord = (int*)calloc(n, sizeof(int));

	/* sort the items */
	sort(n, w[1], ord);
	for (i = 0; i < n; i++) for (j = 0; j < 2; j++) pw[j][i] = w[j][ord[i]];

	/* next fit packing */
	hleft = W[1] - pw[1][0]; wleft = W[0]; 
	ub = 0; hcurr = 0;
	for (i = 0; i < n; i++) {
	    if (pw[0][i] <= wleft) {
	       px[0][i] = W[0] - wleft;
	       wleft -= pw[0][i];
	    } else {
	       if (pw[1][i] <= hleft) {
	          hcurr = W[1] - hleft;
	          hleft -= pw[1][i];
	       } else {
	          hcurr = 0;
	          hleft = W[1] - pw[1][i];
	          if (ub+1 == maxb) goto esc;
	          ub++;
	       }
	       px[0][i] = 0;
	       wleft = W[0] - pw[0][i];
	    }
	    px[1][i] = hcurr;
	    pb[i] = ub;
	}

esc:
	for (i = 0; i < n; i++) {
	    for (j = 0; j < 2; j++) x[j][ord[i]] = px[j][i];
	    b[ord[i]] = pb[i];
	}

	for (i = 0; i < 2; i++) {
	    free(pw[i]);
	    free(px[i]);
	}
	free(pw); free(px); free(pb); free(ord);

	return (ub+1);
}

/* hybrid-hybrid next fit for 3D bin packing */ 
int HHnextFit(
	int  n, int **w, int *W, int **x, int *b
    )
{
	int   i, ub, hcurr, hmax, count, nToPack, nTo, ub0;

	int   **pw, **px, *pb;
	int   *pW, *set;

	pW = (int*)calloc(2, sizeof(int));
	pW[0] = W[1]; pW[1] = W[2];

	for (i = 0; i < n; i++) b[i] = -1;
	nToPack = n;

	ub = 0; hcurr = 0;
	while (nToPack) {
	      pw = (int**)calloc(2, sizeof(int*));
	      px = (int**)calloc(2, sizeof(int*));
	      for (i = 0; i < 2; i++) {
	          pw[i] = (int*)calloc(nToPack, sizeof(int));
	          px[i] = (int*)calloc(nToPack, sizeof(int));
	      }
	      pb = (int*)calloc(nToPack, sizeof(int));
	      set = (int*)calloc(nToPack, sizeof(int));
	      for (i = 0; i < nToPack; i++) pb[i] = -1;
	      count = 0;
	      for (i = 0; i < n; i++) {
	          if (b[i] < 0) {
	             pw[0][count] = w[0][i];
	             pw[1][count] = w[2][i];
	             set[count] = i;
	             count++;
	          }
	      }
	      /* just pack the first 2D bin */
	      ub0 = HnextFit(nToPack, pw, pW, px, pb, 1);
              nTo = nToPack; hmax = 0;
	      for (i = 0; i < nToPack; i++) {
	          if (pb[i] >= 0) {
	             x[0][set[i]] = px[0][i];
	             x[2][set[i]] = px[1][i];
	             if (w[1][set[i]] > hmax) hmax = w[1][set[i]];
	             nTo--;
	          }
	      }
	      if (hcurr+hmax > W[1]) {
	         hcurr = 0;
	         ub++;
	      }
	      for (i = 0; i < nToPack; i++) {
	          if (pb[i] >= 0) {
	             x[1][set[i]] = hcurr;
	             b[set[i]] = ub;
	          }
	      }
	      hcurr += hmax;
	      nToPack = nTo;
	      for (i = 0; i < 2; i++) {
	          free(pw[i]);
	          free(px[i]);
	      }
	      free(pw); free(px); free(pb); free(set);
	}
	free(pW);
	    
	return (ub+1);
}

/* SECTION d): specific utility procedures */

/* determine the next k-tuple of bins */
int nextKT(
	int *kb, int K, int t, int nb
    ) 
{
	int  i, j, h, index;
	int  start = 1;

	i = index = 0;
	if ((K >= nb) || (K == 0)) return 0;

	kb[t] = 0;
	for (i = nb-1; i >= 0; i--) {
	    if (kb[i] == 1) {
	       start = 0;
	       for (j = i+1; j < nb; j++) if ((kb[j] == 0) && (j !=t)) { 
	                                     kb[j] = 1; 
	                                     break; 
	                                  }
	       if (j != nb) {
	          kb[i] = 0; 
	          j++;
	          for (h = nb-1; h>j;) {
	              if (kb[h] == 1) {
	                 if ((kb[j] == 0) && (j != t)) { 
	                    kb[h--] = 0; 
	                    kb[j++] = 1; 
	                 } else j++;
	              } else h--;
	          }
	          return 1;
	      }
	   }
	}
	if (start) {
	   index = i = 0;
	   while (i < nb) {
	         if ((i != t) && (index < K)) { 
	            kb[i++] = 1; 
	            index++; 
	            if (index == K) return 1; 
			 } else kb[i++] = 0;
	   }
	   return 1;
	}
	for (h = 0; h < nb; h++) kb[h] = 0;
	return 0;	
}

/* select the target bin */
int target(
	int D, double *ff, int n
    )
{
	int    i, j, k, t, *tb;
	double *c;

	tb = (int*)calloc(n, sizeof(int));
	c  = (double*)calloc(n, sizeof(double));

	tb[0] = 0; c[0] = ff[0];
	for (k = 1; k < D; k++) { 
	    c[k] = INFINITY; 
	    tb[k] = 0; 
	}

	for (i = 1; i < n; i++) {
	    for (j = 0; j < D; j++) {
	        if (c[j] - ff[i] > EPS) {
	           for (k = j+1; k < D; k++) { 
	               c[k] = c[k-1]; 
	               tb[k] = tb[k-1]; 
	           }
	           c[j] = ff[i]; 
	           tb[j] = i;
	        }
	    }
	}
	t = tb[D-1];
	free(tb); free(c);

        return t;
}

/* check the feasibility of a d-dimensional bin packing solution */
int checkfs(
        int d, int n, int **w, int *W, int **x, int *b
    )
{
        int   i, j, k, count;

        for (i = 0; i < n; i++) {
            for (j = 0; j < d; j++) if (x[j][i]+w[j][i] > W[j]) return 0;
            for (j = i+1; j < n; j++) {
                if (b[i] != b[j]) continue;
                count = 0;
                for (k = 0; k < d; k++)
                 if (intsz(x[k][i], x[k][i]+w[k][i], x[k][j], x[k][j]+w[k][j])) count++;
                if (count == d) return 0;
            }
        }
        return 1;
}

/* SECTION e): general utility procedures */
void sort(
	int n, int *v, int *o
     )
{
	int   *t, h, k;

	t = (int*)calloc(n, sizeof(int));

	for (k = 0; k < n; k++) t[k] = o[k] = 0;
	for (k = 0; k < n; k++) {
	   for (h = k+1; h < n; h++) { 
	       if (v[h] > v[k]) t[k]++;
	       else             t[h]++; 
	   }
	   o[t[k]] = k;
	}
	free(t);
}

void second(
	float *tempo
     )
{
	*tempo = 0.0;
}
