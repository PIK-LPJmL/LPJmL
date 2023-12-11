# Programming style guide for LPJmL

## File header

Each source file should begin with the following header:

```c
/**************************************************************************************/
/**                                                                                \n**/
/**                      f  i  l  e  n  a  m  e  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Description of the function defined in the source code                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/
```

## Indentation style

LPJmL uses the [Allman style](https://en.wikipedia.org/wiki/Indentation_style#Allman_style) for indentation:

```c
while (x == y)
{
  if (x<10)
    something();
  else
  {
    somethingelse();
    morethingstodo();
  }
}
```

Indents are 2 spaces, tabulators should not be used.

## Naming of data types

Data types should begin with an uppercase character:

```c
typedef struct
{
  int variable; /**< description (unit) */
} Datatype;     /**< description of data type */

Datatype variable;
```

## Function headers

Each argument of a function should be on a separate line and commented suitable for [Doxygen](https://www.doxygen.nl/):

```c
Returntype function(Type1 arg1,/**< description of argument 1 (unit) */
                    Type2 arg2 /**< description of argument 2 (unit) */
                   )           /** \return description of return value (unit) */
{
} /* of 'function' */
```

Functions should be implemented in separate source files.
