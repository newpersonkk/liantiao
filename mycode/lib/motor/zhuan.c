#include "motor.h"
#include "param.h"

//up是2  down是5
void noneS0()
{
    mygantry.gantrypos.degree = S0;
    MIkp = 1.0;
    MIkd = 0.5;
}
void noneS90()
{
    mygantry.gantrypos.degree = S90;
    MIkp = 1.0;
    MIkd = 0.5;
}
void noneS180()
{
    mygantry.gantrypos.degree = S180;
    MIkp = 1.0;
    MIkd = 0.5;
}
void noneS270()
{
    mygantry.gantrypos.degree = S270;
    MIkp = 1.0;
    MIkd = 0.5;
}
void oneS0()
{
    mygantry.gantrypos.degree = S0;
    MIkp = 1.8;
    MIkd = 1.5;
}
void oneS0up()
{
    mygantry.gantrypos.degree = S0+0.6*errorth;
    MIkp = 1.8;
    MIkd = 1.5;
}
void oneS0down()
{
    mygantry.gantrypos.degree = S0-0.6*errorth;
    MIkp = 1.8;
    MIkd = 1.5;
}
void oneS90()
{
    mygantry.gantrypos.degree = S90;
    MIkp = 1.8;
    MIkd = 1.5;
}
void oneS180()
{
    mygantry.gantrypos.degree = S180;
    MIkp = 1.8;
    MIkd = 1.5;
}
void oneS180up()
{
    mygantry.gantrypos.degree = S180+0.6*errorth;
    MIkp = 1.8;
    MIkd = 1.5;
}
void oneS180down()
{
    mygantry.gantrypos.degree = S180-0.6*errorth;
    MIkp = 1.8;
    MIkd = 1.5;
}
void oneS270()
{
    mygantry.gantrypos.degree = S270;
    MIkp = 1.8;
    MIkd = 1.5;
}
void twoS0()
{
    mygantry.gantrypos.degree = S0;
    MIkp = 2.2;
    MIkd = 1.5;
}
void twoS0up()
{
    mygantry.gantrypos.degree = S0+0.6*errorth;
    MIkp = 2.2;
    MIkd = 1.5;
}
void twoS0down()
{
    mygantry.gantrypos.degree = S0-0.6*errorth;
    MIkp = 2.2;
    MIkd = 1.5;
}
void twoS90()
{
    mygantry.gantrypos.degree = S90;
    MIkp = 2.2;
    MIkd = 1.5;
}
void twoS180()
{
    mygantry.gantrypos.degree = S180;
    MIkp = 2.2;
    MIkd = 1.5;
}
void twoS180up()
{
    mygantry.gantrypos.degree = S180+0.6*errorth;
    MIkp = 2.2;
    MIkd = 1.5;
}
void twoS180down()
{
    mygantry.gantrypos.degree = S180-0.6*errorth;
    MIkp = 2.2;
    MIkd = 1.5;
}
void twoS270()
{
    mygantry.gantrypos.degree = S270;
    MIkp = 2.2;
    MIkd = 1.5;
}