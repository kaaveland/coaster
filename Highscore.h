#ifndef HIGHSCORE_H
#define HIGHSCORE_H 1

#include <fstream>
#include <string>
#include <Ogre.h>

class Highscore
{
private:
     Ogre::Real best;

     std::string fname;
public:
     Highscore(std::string);
     
     Ogre::Real getBest();
     void enterScore(Ogre::Real);
};

#endif /* HIGHSCORE_H */
