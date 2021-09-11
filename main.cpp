#include <iostream>
#include <thread>
#include <math.h>
#include <future>
#include <memory>

#include <immintrin.h>
// This is the GCC and CLANG header file for non-SIMD intrinsics, it seems.  But the above Intel one also works
// on linux.
#include <x86intrin.h>

#include <stdio.h>
#include <assert.h>

#include <algorithm>
#include <iostream>

// These two functions should be usable.
inline auto start_tsc() {
    _mm_lfence();
    auto tsc = __rdtsc();
    _mm_lfence();
    return tsc;
}

inline auto stop_tsc() {
    unsigned int aux;
    auto tsc = __rdtscp(&aux);
    _mm_lfence();
    return tsc;
}



double integralFunction(double x){          //this is just a function that we are going to integrate
    if(x == 0)
        return 1;   //lim(sin(x)/x) = 0; This prevents division by zero errors and gives correct value
    return double(sin(x)/x);
}

double integrate(double lowerBound, double upperBound, int numOfSections){           //usage: ./integrate lower upper nSecs nThreads
    /*
    lower limit = argv[1]
    upper limit = argv[2]
    numOfSections = argv[3]
    numOfThreads = argv[4]
    */

    double integral = 0;


    //double integral = 0;
    double *xVals = new double[numOfSections+1];
    double *yVals = new double[numOfSections+1];

    double width = (upperBound-lowerBound)/numOfSections; 
    //formula: f(x) = (b-a)/2n  * [f(a) + 2*f(a+n) + ... + 2*f(b-n) + f(b)]
    //This will be the basis of the formula I use to calculate the integral. 
    //If N is high enough, then it should be fairly accurate

    for (int i=0; i<=numOfSections; i++)            
    {                    
        xVals[i]=lowerBound+i*width;    //value of x to be passed to the function         
        yVals[i] = integralFunction(xVals[i]);  //calculating height of block
        if(i > 0 && i < numOfSections){     
            integral+= width*yVals[i];          //width*y[i] is the area of the current block. Add block to integral
        }
    }

    integral+=width/2.0*(yVals[(int)lowerBound]+yVals[(int)upperBound]); //f(a) and f(b) from the formula listed above

    //std::cout<< integral << std::endl;
    delete xVals;
    delete yVals;

    return integral;
}

void helper(double lowerBound, double upperBound, int numOfSections, int index, double* vals){
    vals[index] = integrate(lowerBound, upperBound, numOfSections);
}

int main(int argc, char* argv[]){

    //unsigned long long start = start_tsc();

    argc += 1; //Get warning for unused parameter. I need to keep it in main because I need argv

    float lowerBound = atof(argv[1]);
    float upperBound = atof(argv[2]);
    int numOfSections = atoi(argv[3]);
    int numOfThreads = atoi(argv[4]);

    std::unique_ptr<std::thread[]> arr(new std::thread[numOfThreads]);

    double incrementor = (upperBound-lowerBound)/numOfThreads;
    double previousPoint = lowerBound;

    double *vals = new double[numOfThreads];

    for(int i = 0; i < numOfThreads; i++){
        arr[i] = std::thread(helper, previousPoint, previousPoint+incrementor, numOfSections/numOfThreads, i, vals);
        previousPoint+= incrementor;
    }                    

    for(int i = 0; i < numOfThreads; i++){
        arr[i].join();
    }
    
    double sum = 0;

    for(int i = 0; i < numOfThreads; i++){
        sum += vals[i];
    }
    
    delete vals;

    std::cout << sum << std::endl;

    //unsigned long long end = stop_tsc();

    //std::cout << double(end - start)/(double(1800000000)) << std::endl;
}