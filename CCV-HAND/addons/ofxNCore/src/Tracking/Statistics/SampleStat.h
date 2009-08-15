#ifndef _SAMPLESTAT_H
#define _SAMPLESTAT_H


class sampleStat
{
public:
		float* sampleSet;
		int N;
		float Sum;

		float sum()
		{
			Sum = 0;
			for (int i=0; i<N; i++)
			{
				Sum += sampleSet[i];
				printf("Sum %i, %f\n", i, Sum);
			}
			printf("Sum Final%f\n", Sum);
			return Sum;
		}

		float sqm()
		{
			float Mean, Sqm;
			Sqm = 0;
			Mean = getMean();
			for (int i=0; i<N; i++)
				Sqm+=(sampleSet[i]-Mean)*(sampleSet[i]-Mean);
			return Sqm;
			}

		void setValue (float x, int i){
			sampleSet[i]=x;
			printf("SampleSet[i]:%f\n", sampleSet[i]);
		}

		sampleStat(int n){
			N = n;
			sampleSet = new float[N];
		}

		float getMean()
		{
			float mean=0;

			mean = sum()/N;

			printf("Mean: %f\n", mean);

			return	mean;
		}

		float getVariance(){
			float Sqm;
			Sqm = sqm();
			printf("SQM %f \n", Sqm);

			return Sqm/(N-1);
		}

		float getStdDeviation(){
			return sqrt(getVariance());
		}
};
#endif
