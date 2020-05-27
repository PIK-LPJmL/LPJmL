/***************************************************************************/
/**                                                                       **/
/**        e f r.  c                                                      **/
/**																          **/	
/**     Copy version of Potsdam by Lotte - trial                          **/
/**																          **/
/**     C implementation of LPJ                                           **/
/**                                                                       **/
/**     Function performs alrithms of 3 environmental flow methods        **/
/**                                                                       **/
/**        Details of EF methods algorithm in: Pastor, A. V., Ludwig, F., **/
/**   Biemans, H., Hoff, H., and Kabat, P.:                               **/
/**   Accounting for environmental flow requirements in                   **/
/**   global water assessments, Hydrol. Earth Syst. Sci. Discuss.,        **/
/**   10, 14987-15032, doi:10.5194/hessd-10-14987-2013, 2013.             **/
/**                                                                       **/
/**      written by Amandine Pastor, Wietse Franssen                      **/
/**     Wageningen univeristy, Earth system science                       **/
/**     Postbus 47                                                        **/
/**     6700AA Wageningen, Netherlands                                    **/                                             
/**                                                                       **/
/**                                                                       **/
/**     Last change:  17.06.2019                                          **/
/**                                                                       **/
/***************************************************************************/
#include "lpj.h"
//#include "efr.h"


#define checkptr(ptr) if(ptr==NULL) { return TRUE; }

static int compare(const Real * a, const Real * b)
{
	if (*a == *b)
		return 0;
	else
		if (*a < *b)
			return -1;
		else
			return 1;
} /* of 'compare' *//*Note Lotte: used for sorting the array of mdischarge_buffer in ascending order*/

// ook bij newgrid.c aangepast
//newgrid is wel drastisch anders bij de 2 verschillende versies. Misschien toch nog ook met andere versie vergelijken?
Bool new_efrdata(Efrdata *efr, Bool notfromrestart)
{
	int m;

	efr->discharge_q90 = 0.0;
	efr->discharge_q75 = 0.0;
	efr->discharge_q50 = 0.0;
	efr->adischarge_mean = 0.0;
	efr->aefr_target = 0.0;
	efr->efr_buffer = 0.0;
	efr->mdischarge_mean = newvec(Real, NMONTH);
	checkptr(efr->mdischarge_mean);
	efr->mefr_target = newvec(Real, NMONTH);
	checkptr(efr->mefr_target);
	efr->mefr_deficit = newvec(Real, NMONTH);
	checkptr(efr->mefr_deficit);
	efr->mefr_threshold = newvec(Real, NMONTH);
	checkptr(efr->mefr_threshold);
	for (m = 0;m<NMONTH;m++)
	{
		efr->mdischarge_mean[m] = 0.0;
		efr->mefr_target[m] = 0.0;
		efr->mefr_deficit[m] = 0.0;
		efr->mefr_threshold[m] = 0.0;
	}
	if (notfromrestart)
	{
		efr->mdischarge_buffer = newvec(Real, NMONTH*NYEAR_EFR);// hier wordt mdischarge binnen efr als vector gedefineerd. 
		checkptr(efr->mdischarge_buffer);
		for (m = 0;m<NMONTH*NYEAR_EFR;m++)
			efr->mdischarge_buffer[m] = 0.0;
	}
	return FALSE;
} /* of 'init_efrdata' */

/*At the end of restart data calculated in discharge_statistics is written in fwritecell.c, so that it can be used in the simulation (for efr_determine)*/
Bool fwriteefrdata(FILE *file, const Efrdata *efr)
{
	fwrite(&efr->discharge_q90, sizeof(Real), 1, file);
	fwrite(&efr->discharge_q75, sizeof(Real), 1, file);
	fwrite(&efr->discharge_q50, sizeof(Real), 1, file);
	fwrite(&efr->adischarge_mean, sizeof(Real), 1, file);
	return fwrite(efr->mdischarge_mean, sizeof(Real), NMONTH, file);
} /* of 'fwriteefrdata' */

// is dat swap het kopieren van content naar iets anders? Snap er niks van 
Bool freadefrdata(FILE *file, Efrdata *efr, Bool swap)
{
	freadreal1(&efr->discharge_q90, swap, file);// freadreal1=freadreal(data,1,swap,file);
	freadreal1(&efr->discharge_q75, swap, file);
	freadreal1(&efr->discharge_q50, swap, file);
	freadreal1(&efr->adischarge_mean, swap, file);
	return freadreal(efr->mdischarge_mean, NMONTH, swap, file) != NMONTH;
} /* of 'freadefrdata' */


// gegevens komen uit efr. Wat dan met cell? In cell is efrdata dan weer gedefineerd als efr? of efr verwijzing naar efrdata dan.  
Bool discharge_statistics(Cell *cell)
{
	int y, m, i;
	int arraySize;
	Real *sortArray;

	cell->efr.adischarge_mean = 0.0;// Waarom kan dit niet?: Efrdata.adischarge_mean = 0.0; Efrdata is dus alleen nog maar typedef, net als int etc. efr is dan in cell.h gedeclareerd als de struct. Al die variabelen vallen dan onder dat type. Waarom niet rechtstreeks als efr structuur defineren?
	for (m = 0;m<NMONTH;m++)//discharge buffer wordt bij de mean opgeteld, als nieuwe mean, zoek op waarom. Berekening hieronder ziet er heel onlogisch uit, wat doen ze precies?
	{
		cell->efr.mdischarge_mean[m] = 0.0;
		for (y = 0;y<NYEAR_EFR;y++) // voor elk jaar discharge_mean is discharge_mean + discharge_buffer. Waarom? Lijkt me raar
			cell->efr.mdischarge_mean[m] += cell->efr.mdischarge_buffer[NMONTH*y + m];//mdischarge buffer loopt door voor alle jaren elke maand dus dit is de mdischarge_buffer in dat jaar in maand m. In principe wordt dus voor elke maand een extra buffer erbij opgeteld. waarom weet ik niet. 
		cell->efr.mdischarge_mean[m] /= NYEAR_EFR; /* 1e6 m3 per day *///Zelfde soort berekening als die hieronder denk ik, maar waarom dan delen door nyear_efr? Je berekent het voor elke maand in elk jaar en dan bepaal je het gemiddelde over die 30 jaar. vervolgens kijk je voor elk jaar aan de hand van de monthly discharges naar annual discharge. Misschien klopt het dus maar wel beetje apart. 
		cell->efr.adischarge_mean += cell->efr.mdischarge_mean[m] * ndaymonth[m];//annual mean discharge is som van mdischarge_mean*dagen van die maand van elke amaand opgeteld en dan gedeeld door dagen jaar. klopt. 
	}
	cell->efr.adischarge_mean /= NDAYYEAR; /* 1e6 m3 per day */
	/*Mean monthly discharge per dag. */
  arraySize = NMONTH*NYEAR_EFR;
	sortArray = newvec(Real, arraySize);
	checkptr(sortArray);
	for (i = 0;i<arraySize;i++)
		sortArray[i] = cell->efr.mdischarge_buffer[i];
	qsort(sortArray, arraySize, sizeof(Real), compare);

  /*Note Lotte: Array of mdischarge_buffer is sorted in ascending order, to determine the discharge that is exceeded 90,75 and 50% of the time. Addition of 0.5 is for rounding errors?*/
  cell->efr.discharge_q90 = sortArray[(int)(NYEAR_EFR*NMONTH*0.1 + 0.5)];                     
	cell->efr.discharge_q75 = sortArray[(int)(NYEAR_EFR*NMONTH*0.25 + 0.5)];
	cell->efr.discharge_q50 = sortArray[(int)(NYEAR_EFR*NMONTH*0.5 + 0.5)];

	return TRUE;
} /* of 'discharge_statistics' */

Bool efr_determine(Cell grid[], int ngridcell, int efroption)
{
  int m, cell;

	switch (efroption)
	{
	case 0: /* If EFR is turned off set monthly targets and limits to zero */
		for (cell = 0;cell<ngridcell;cell++)
		{
			/* no EFRs: nothing hold back from river withdrawal, yet EFR_target is defined (after VMF_MAX method) to measure EFR deficits if EFRs were not respected */
			grid[cell].efr.aefr_target = 0.0;
			for (m = 0;m<NMONTH;m++)
			{
				if (grid[cell].efr.mdischarge_mean[m] <= 0.4*grid[cell].efr.adischarge_mean)
					grid[cell].efr.mefr_target[m] = 0.75*grid[cell].efr.mdischarge_mean[m];
				else if (grid[cell].efr.mdischarge_mean[m]>0.8*grid[cell].efr.adischarge_mean)
					grid[cell].efr.mefr_target[m] = 0.45*grid[cell].efr.mdischarge_mean[m];
				else
					grid[cell].efr.mefr_target[m] = 0.6*grid[cell].efr.mdischarge_mean[m];

				grid[cell].efr.mefr_threshold[m] = grid[cell].efr.mefr_target[m] * 1e9; /* (liter per day) */
				grid[cell].efr.aefr_target += grid[cell].efr.mefr_target[m] * ndaymonth[m]; /* (km3 per year) */
			}
			grid[cell].efr.aefr_target /= NDAYYEAR;

		}
		break;

	case 1: /* Variable Monthly Flow method (VMF_MIN) minus uncertainty range (-15%) of mean monthly flow (see Steffen et al. 2015) */
		for (cell = 0;cell<ngridcell;cell++)
		{
			grid[cell].efr.aefr_target = 0.0;
			for (m = 0;m<NMONTH;m++)
			{
				if (grid[cell].efr.mdischarge_mean[m] <= 0.4*grid[cell].efr.adischarge_mean)
					grid[cell].efr.mefr_target[m] = 0.45*grid[cell].efr.mdischarge_mean[m];
				else if (grid[cell].efr.mdischarge_mean[m]>0.8*grid[cell].efr.adischarge_mean)
					grid[cell].efr.mefr_target[m] = 0.15*grid[cell].efr.mdischarge_mean[m];
				else
					grid[cell].efr.mefr_target[m] = 0.3*grid[cell].efr.mdischarge_mean[m];

				grid[cell].efr.mefr_threshold[m] = grid[cell].efr.mefr_target[m] * 1e9; /* (liter per day) */
				grid[cell].efr.aefr_target += grid[cell].efr.mefr_target[m] * ndaymonth[m]; /* (km3 per year) */
			}
			grid[cell].efr.aefr_target /= NDAYYEAR;
		}
		break;

	case 2: /* Variable Monthly Flow method (VMF) */
		for (cell = 0;cell<ngridcell;cell++)
		{
			grid[cell].efr.aefr_target = 0.0;
			for (m = 0;m<NMONTH;m++)
			{
				if (grid[cell].efr.mdischarge_mean[m] <= 0.4*grid[cell].efr.adischarge_mean)
					grid[cell].efr.mefr_target[m] = 0.6*grid[cell].efr.mdischarge_mean[m];
				else if (grid[cell].efr.mdischarge_mean[m]>0.8*grid[cell].efr.adischarge_mean)
					grid[cell].efr.mefr_target[m] = 0.3*grid[cell].efr.mdischarge_mean[m];
				else
					grid[cell].efr.mefr_target[m] = 0.45*grid[cell].efr.mdischarge_mean[m];

				grid[cell].efr.mefr_threshold[m] = grid[cell].efr.mefr_target[m] * 1e9; /* (liter per day) */
				grid[cell].efr.aefr_target += grid[cell].efr.mefr_target[m] * ndaymonth[m]; /* (km3 per year) */
			}
			grid[cell].efr.aefr_target /= NDAYYEAR;
		}
		break;

	case 3: /* Variable Monthly Flow method (VMF_MAX) including uncertainty range of +15% of mean monthly flow (see Steffen et al. 2015) */
		for (cell = 0;cell<ngridcell;cell++)
		{
			grid[cell].efr.aefr_target = 0.0;
			for (m = 0;m<NMONTH;m++)
			{
				if (grid[cell].efr.mdischarge_mean[m] <= 0.4*grid[cell].efr.adischarge_mean)
					grid[cell].efr.mefr_target[m] = 0.75*grid[cell].efr.mdischarge_mean[m];
				else if (grid[cell].efr.mdischarge_mean[m]>0.8*grid[cell].efr.adischarge_mean)
					grid[cell].efr.mefr_target[m] = 0.45*grid[cell].efr.mdischarge_mean[m];
				else
					grid[cell].efr.mefr_target[m] = 0.6*grid[cell].efr.mdischarge_mean[m];

				grid[cell].efr.mefr_threshold[m] = grid[cell].efr.mefr_target[m] * 1e9; /* (liter per day) */
				grid[cell].efr.aefr_target += grid[cell].efr.mefr_target[m] * ndaymonth[m]; /* (km3 per year) */
			}
			grid[cell].efr.aefr_target /= NDAYYEAR;
		}
		break;

	case 4: /* Smakhtin method (SMAK_EFR) */
		for (cell = 0;cell<ngridcell;cell++)
		{
			grid[cell].efr.aefr_target = 0.0;
			for (m = 0;m<NMONTH;m++)
			{
				if (grid[cell].efr.discharge_q90>0.3*grid[cell].efr.adischarge_mean)
					grid[cell].efr.mefr_target[m] = grid[cell].efr.discharge_q90;
				else if (grid[cell].efr.discharge_q90>0.2*grid[cell].efr.adischarge_mean)
					grid[cell].efr.mefr_target[m] = grid[cell].efr.discharge_q90 + 0.07*grid[cell].efr.adischarge_mean;
				else if (grid[cell].efr.discharge_q90>0.1*grid[cell].efr.adischarge_mean)
					grid[cell].efr.mefr_target[m] = grid[cell].efr.discharge_q90 + 0.15*grid[cell].efr.adischarge_mean;
				else
					grid[cell].efr.mefr_target[m] = grid[cell].efr.discharge_q90 + 0.2*grid[cell].efr.adischarge_mean;
				grid[cell].efr.mefr_threshold[m] = grid[cell].efr.mefr_target[m] * 1e9;
				grid[cell].efr.aefr_target += grid[cell].efr.mefr_target[m] * ndaymonth[m];
			}
			grid[cell].efr.aefr_target /= NDAYYEAR;
		}
		break;

	case 5: /* Smakhtin method (SMAK_Q50_EFR) */
		for (cell = 0;cell<ngridcell;cell++)
		{
			grid[cell].efr.aefr_target = 0.0;
			for (m = 0;m<NMONTH;m++)
			{
				if (grid[cell].efr.mdischarge_mean[m] <= 0.8*grid[cell].efr.adischarge_mean) /* low flow (including intermediate) */
				{
					if (grid[cell].efr.discharge_q90>0.3*grid[cell].efr.adischarge_mean)
						grid[cell].efr.mefr_target[m] = grid[cell].efr.discharge_q90;
					else if (grid[cell].efr.discharge_q90>0.2*grid[cell].efr.adischarge_mean)
						grid[cell].efr.mefr_target[m] = grid[cell].efr.discharge_q90 + 0.07*grid[cell].efr.adischarge_mean;
					else if (grid[cell].efr.discharge_q90>0.1*grid[cell].efr.adischarge_mean)
						grid[cell].efr.mefr_target[m] = grid[cell].efr.discharge_q90 + 0.15*grid[cell].efr.adischarge_mean;
					else
						grid[cell].efr.mefr_target[m] = grid[cell].efr.discharge_q90 + 0.2*grid[cell].efr.adischarge_mean;
				}
				else /* high flow */
				{
					if (grid[cell].efr.discharge_q90>0.3*grid[cell].efr.adischarge_mean)
						grid[cell].efr.mefr_target[m] = grid[cell].efr.discharge_q50;
					else if (grid[cell].efr.discharge_q90>0.2*grid[cell].efr.adischarge_mean)
						grid[cell].efr.mefr_target[m] = grid[cell].efr.discharge_q50 + 0.07*grid[cell].efr.adischarge_mean;
					else if (grid[cell].efr.discharge_q90>0.1*grid[cell].efr.adischarge_mean)
						grid[cell].efr.mefr_target[m] = grid[cell].efr.discharge_q50 + 0.15*grid[cell].efr.adischarge_mean;
					else
						grid[cell].efr.mefr_target[m] = grid[cell].efr.discharge_q50 + 0.2*grid[cell].efr.adischarge_mean;
				}
				if (grid[cell].efr.mefr_target[m]>0.8*grid[cell].efr.mdischarge_mean[m]) /* annual quantiles can exceed monthly flow, set to maximal 80% */
					grid[cell].efr.mefr_target[m] = 0.8*grid[cell].efr.mdischarge_mean[m];
				grid[cell].efr.mefr_threshold[m] = grid[cell].efr.mefr_target[m] * 1e9;
				grid[cell].efr.aefr_target += grid[cell].efr.mefr_target[m] * ndaymonth[m];
			}
			grid[cell].efr.aefr_target /= NDAYYEAR;
		}
		break;

	case 6: /* Tessmann method (TESSMANN_EFR) */
		for (cell = 0;cell<ngridcell;cell++)
		{
			grid[cell].efr.aefr_target = 0.0;
			for (m = 0;m<NMONTH;m++)
			{
				if (grid[cell].efr.mdischarge_mean[m]>grid[cell].efr.adischarge_mean)
					grid[cell].efr.mefr_target[m] = 0.4*grid[cell].efr.mdischarge_mean[m]; /* high-flow: 40% MMF */
				else if (grid[cell].efr.mdischarge_mean[m]>0.4*grid[cell].efr.adischarge_mean)
					grid[cell].efr.mefr_target[m] = 0.4*grid[cell].efr.adischarge_mean; /* intermediate-flow: 40% MAF */
				else
					grid[cell].efr.mefr_target[m] = 0.8*grid[cell].efr.mdischarge_mean[m]; /* low-flow adopted from Richter 2012: 80%, original Tessmann: 100% MMF*/
				grid[cell].efr.mefr_threshold[m] = grid[cell].efr.mefr_target[m] * 1e9;
				grid[cell].efr.aefr_target += grid[cell].efr.mefr_target[m] * ndaymonth[m];
			}
			grid[cell].efr.aefr_target /= NDAYYEAR;
		}
		break;

	case 7: /* Richter method (RICHTER_EFR) */
		for (cell = 0;cell<ngridcell;cell++)
		{
			grid[cell].efr.aefr_target = 0.0;
			for (m = 0;m<NMONTH;m++)
			{
				grid[cell].efr.mefr_target[m] = 0.8*grid[cell].efr.mdischarge_mean[m]; /* always 80% MMF */
				grid[cell].efr.mefr_threshold[m] = grid[cell].efr.mefr_target[m] * 1e9;
				grid[cell].efr.aefr_target += grid[cell].efr.mefr_target[m] * ndaymonth[m];
			}
			grid[cell].efr.aefr_target /= NDAYYEAR;
		}
		break;
	}
	return TRUE;
} /* of 'efr_determine' */
