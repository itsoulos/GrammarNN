#include "triangularsampler.h"

TriangularSampler::TriangularSampler(IntervalProblem *p)
    :ProblemSampler("triangular",p)
{

}

double  TriangularSampler::eval(Data &xpoint)
{
    return 0.0;
}

void    TriangularSampler::addSampleFromProblem(Data &x,double y)
{
    //nothing here
}

void    TriangularSampler::sampleFromProblem(int N,Matrix &xsample,Data &ysample)
{
    int i;
      xpoint.resize(N);
      ypoint.resize(N);
      int n =myProblem->getDimension();
      Data  c, F;

      c.resize(n);
      F.resize(n);
      IntervalData m = myProblem->getMargins();

      //printf("n = %i\n",n);
      for (int j = 0; j < n; j++)
      {
          double a = m[j].leftValue();
          double b = m[j].rightValue();
          c[j] = (a + b) / 2.;
          F.at(j) = (c[j]-a) / (b-a);
      }
      Data point;
      point.resize(n);
      for (int i = 0; i < N; i++)
      {
          xpoint.at(i).resize(n);
          xpoint.at(i) = myProblem->getSample();


          for (int j = 0; j < n; j++)
          {
              double a = m[j].leftValue();
              double b = m[j].rightValue();
              double U = rand() / (double)RAND_MAX;
              if (U <= F[j])
                  point.at(j) = a + sqrt(U * (b - a) * (c[j] - a));
              else
                  point.at(j) = b - sqrt((1. - U) * (b - a) * (b - c[j]));
          }
          xpoint.at(i) = point;
          ypoint.at(i) = myProblem->funmin(xpoint.at(i));
      }
      xsample = xpoint;
      ysample = ypoint;

}
void    TriangularSampler::sampleFromModel(int &N,Matrix &xsample,Data &ysample)
{
    xsample = xpoint;
    ysample = ypoint;
}

TriangularSampler::~TriangularSampler()
{

}

