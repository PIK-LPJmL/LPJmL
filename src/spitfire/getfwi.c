/**************************************************************************************/
/**                                                                                \n**/
/**               g  e  t  f  w  i  .  c                                           \n**/
/**                                                                                \n**/
/**     Function calculates Canadian fire weather index                            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real getfwi(FWIdata *fwi,
            const Coord *coord,           /**< cell coordinate */
            const Dailyclimate  *climate, /**< daily climate data */
            int month,                    /**< month (0..11) */
            Bool relative_humidity        /**< humidity is relative humidity (TRUE/FALSE) */
           )                              /** \return Canadian fire weather index */
{
  Real d_fdi;
  Real temperature, rh,rk,cc,bb,isi,fW,fF,dc1,smi,pr;
  Real wmo,ed,ew,bui0,bui1,x,z,wm,rw,fm,wmr,pr0,dr0,pe,dr,wmi,be,pe1;
  Real ra;
/*46N: Canadian standard, latitude >= 30N   (Van Wagner 1987)*/
  static Real ell01[NMONTH] = {6.5, 7.5, 9, 12.8, 13.9, 13.9, 12.4, 10.9, 9.4, 8, 7, 6};
/*20N: For 30 > latitude >= 10*/
  static Real ell02[NMONTH] = {7.9, 8.4, 8.9, 9.5, 9.9, 10.2, 10.1, 9.7, 9.1,8.6, 8.1, 7.8};
/*20S: For -10 > latitude >= -30 */
  static Real ell03[NMONTH] = {10.1, 9.6, 9.1, 8.5, 8.1, 7.8, 7.9, 8.3, 8.9, 9.4, 9.9, 10.2};
/*40S: For -30 > latitude*/
  static Real ell04[NMONTH] = {11.5, 10.5, 9.2, 7.9, 6.8, 6.2, 6.5, 7.4, 8.7, 10, 11.2, 11.8};
/*20N: North of 20 degrees N*/
  static Real fl01[NMONTH] = {-1.6, -1.6, -1.6, 0.9, 3.8, 5.8, 6.4, 5, 2.4, 0.4, -1.6, -1.6};
/*20S: South of 20 degrees S*/
  static Real fl02[NMONTH] = {6.4, 5, 2.4, 0.4, -1.6, -1.6, -1.6, -1.6, -1.6, 0.9, 3.8, 5.8};

  if(relative_humidity)
    rh=climate->humid*100; /*in percent*/
  else
    rh=getrh(climate->tmax,climate->humid)*100; /*in percent*/

  wmo = 147.27723 * (101-fwi->ffmc) / (59.5 + fwi->ffmc);
  if (climate->prec > 0.5)
    ra = climate->prec - 0.5;
  else
    ra = climate->prec;
  if (climate->prec > 0.5)
  {
    if(wmo>150)
      wmo += 0.0015 * (wmo - 150) * (wmo - 150) * sqrt(ra) + 42.5 * ra * exp(-100/(251-wmo)) * (1 - exp(-6.93/ra));
    else
      wmo += 42.5*ra*exp(-100.0/(251.-wmo))*(1-exp(-6.93/ra));
  }
  wmo=min(250,wmo);
  /*Eq. 4 Equilibrium moisture content from drying*/
  ed = 0.942 * pow(rh,0.679) + (11 * exp((rh - 100) / 10)) + 0.18 * (21.1 - climate->tmax) * (1 - 1 / exp(rh * 0.115));
  /*Eq. 5 Equilibrium moisture content from wetting*/

  ew = 0.618 * pow(rh,0.753) + (10 * exp((rh - 100) / 10)) + 0.18 * (21.1 - climate->tmax) * (1 - 1 / exp(rh * 0.115));

/*Eq. 6a (ko) Log drying rate at the normal termperature of 21.1 C*/

  z = (wmo < ed && wmo < ew) ?  0.424 * (1 - pow((100 - rh) / 100,1.7)) + 0.0694 * sqrt(climate->windspeed*3.6) * (1 - pow((100 - rh) / 100,8)) : 0;

/*Eq. 6b effect of temperature on  drying rate*/

  x = z * 0.581 * exp(0.0365*climate->tmax);
  wm = (wmo < ed && wmo < ew) ?  ew - (ew - wmo)/pow(10,x) : wmo;

/*Eq. 7a (ko) Log wetting rate at the normal termperature of 21.1 C*/

  z = ( wmo > ed) ?  0.424 * (1 - pow(rh/100,1.7)) + 0.0694 * sqrt(climate->windspeed*3.6) * (1 - pow(rh/100,8)) : z;
/*Eq. 7b Affect of temperature on  wetting rate*/

  x = z * 0.581 * exp(0.0365 * climate->tmax);

/*Eq. 9*/

  wm = (wmo > ed) ?  ed + (wmo - ed)/pow(10,x) : wm;

/*Eq. 10 Final ffmc calculation*/

  fwi->ffmc = (59.5 * (250 - wm))/(147.2 + wm);

/*Constraints*/

  fwi->ffmc = min(fwi->ffmc,101);
  fwi->ffmc = max(fwi->ffmc,0);

/********************************************************************/
/*Duff Moisture Code (DMC)*/

/***constrain low end of temperature***/
  temperature = max(climate->tmax,-1.1);


/***Eq. 16 - The log drying rate***/

  rk = 1.894 * (temperature + 1.1) * (100 - rh) * ell01[month] * 1e-04;

/***Adjust the day length  and thus the drying r, based on latitude and month***/

  rk = (coord->lat <= 30 && coord->lat > 10) ? 1.894 * (temperature + 1.1) * (100 - rh) * ell02[month] * 1e-04 : rk;
  rk = (coord->lat <= -10 && coord->lat > -30) ? 1.894 * (temperature + 1.1) * (100 - rh) * ell03[month] * 1e-04 : rk;
  rk = (coord->lat <= -30 && coord->lat >= -90) ? 1.894 * (temperature + 1.1) * (100 - rh) * ell04[month] * 1e-04 : rk;
  rk = (coord->lat <= 10 && coord->lat > -10) ? 1.894 * (temperature + 1.1) * (100 - rh) * 9 * 1e-04 : rk;
  ra = climate->prec;

/*      **Eq. 11 - Net rain amount***/
  rw = 0.92 * ra - 1.27;

/***Alteration to Eq. 12 to calculate more accurately***/

  wmi = 20 + 280/exp(0.023 * fwi->dmc);

/***Eqs. 13a, 13b, 13c***/

  if (fwi->dmc <= 33)
    be = 100/(0.5 + 0.3 * fwi->dmc);
  else if (fwi->dmc <= 65)
    be = 14 - 1.3 * log(fwi->dmc);
  else
    be = 6.2 * log(fwi->dmc) - 17.2;


/***Eq. 14 - Moisture content after rain**/

  wmr = wmi + 1000 * rw/(48.77 + be * rw);

/***Alteration to Eq. 15 to calculate more accurately**/

  pr0 = 43.43 * (5.6348 - log(wmr - 20));

/**Constrain P**/

  pr = (climate->prec <= 1.5) ? fwi->dmc : pr0;
  pr = max(0,pr);


/**Calculate final P (DMC)**/

  fwi->dmc = max(pr + rk, 0);

/*************************************************************************/
/************Drought Code (DC)*******************************************/

temperature = max(climate->tmax,-2.8);

/***Eq. 22 - Potential Evapotranspiration**/
  pe = (0.36 * (temperature + 2.8) + fl01[month]) / 2;

/***Daylength factor adjustment by latitude for Potential Evapotranspiration**/

  pe = (coord->lat <= -20) ? (0.36 * (temperature + 2.8) + fl02[month]) / 2 : pe;
  pe = (coord->lat <= 20 && coord->lat>-20) ? (0.36 * (temperature + 2.8) + 1.4) / 2 : pe;

/**Cap potential evapotranspiration at 0 for negative winter DC values**/

  pe = max(0,pe);
  ra = climate->prec;

/**Eq. 18 - Effective Rainfall**/

  rw = 0.83 * ra - 1.27;

/**Eq. 19**/

  smi = 800 * exp(-1 * fwi->dc / 400);

/**Alteration to Eq. 21**/

  dr0 = fwi->dc - 400 * log(1 + 3.937 * rw/smi);
  dr0 = max(dr0,0);

/***if precip is less than 2.8 then use yesterday's DC**/

  dr = (climate->prec <= 2.8) ? fwi->dc : dr0;

/**Alteration to Eq. 23**/

  dc1 = dr + pe;
  dc1 = max(0,dc1);
  fwi->dc=dc1;
/******************************************************************/
/***************** Initial spread index****************************/

/**Eq. 10 - Moisture content**/

  fm = 147.2 * (101 - fwi->ffmc)/(59.5 + fwi->ffmc);

/**Eq. 24 - Wind Effect**/
/* the ifelse, also takes care of the ISI modification for the fbp functions*/
/* this modification is *Equation 53a in FCFDG (1992)* */

  fW = (climate->windspeed*3.6 >= 40) ? 12 * (1 - exp(-0.0818 * (climate->windspeed*3.6 - 28))) : exp(0.05039 * climate->windspeed*3.6);

/**Eq. 25 - Fine Fuel Moisture**/

  fF = 91.9 * exp(-0.1386 * fm) * (1 + pow(fm,5.31) / 49300000);

/**Eq. 26 - Spread Index Equation**/

  isi = 0.208 * fW * fF;

/******************************************************************/
/********************* Buildup index*******************************/

/**Eq. 27a**/

  bui1 = (fwi->dmc == 0 && fwi->dc == 0) ? 0 : 0.8 * fwi->dc * fwi->dmc/(fwi->dmc + 0.4 * fwi->dc);

/**Eq. 27b - next 3 lines**/

  pe1 = (fwi->dmc == 0) ? 0 : (fwi->dmc - bui1)/fwi->dmc;

  cc = 0.92 + pow(0.0114 * fwi->dmc,1.7);
  bui0 = fwi->dmc - cc * pe1;

/**Constraints**/

  bui0 = max(0,bui0);
  bui1 = (bui1 < fwi->dmc) ? bui0 : bui1;

/*****************************************************************************/
/*************** Fire weather index (FWI)*************************************/

  bb = (bui1 > 80) ? 0.1 * isi * (1000/(25 + 108.64/exp(0.023 * bui1))) : 0.1 * isi * (0.626 * pow(bui1,0.809) + 2);

/**Eqs. 30b, 30a**/

  d_fdi = (bb <= 1) ? bb : exp(2.72 * pow((0.434 * log(bb)),0.647));
  return d_fdi;
} /* of 'getfwi' */
