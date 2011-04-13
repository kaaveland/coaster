#include "Highscore.h"

Highscore::Highscore(std::string filename)
{
     fname = filename;
     best = 1E10;
}

float Highscore::getBest()
{
     std::ifstream scores(fname.c_str());
     if (!scores.good()) {
          scores.close();
          return best;
     }
     scores >> best;
     scores.close();
     return best;
}

void Highscore::enterScore(float score)
{
     if (score < this->getBest()) {
          best = score;
          std::ofstream out(fname.c_str());
          out << best << std::endl;
          out.close();
     }
}

