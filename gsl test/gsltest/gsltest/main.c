//
//  main.c
//  gsltest
//
//  Created by George Jerman on 26/09/2019.
//  Copyright © 2019 George Jerman. All rights reserved.
//
#include <stdio.h>
#include <gsl/gsl_sf_bessel.h>

int
main (void)
{
  double x = 5.0;
  double y = gsl_sf_bessel_J0 (x);
  printf ("J0(%g) = %.18e\n", x, y);
  return 0;
}
