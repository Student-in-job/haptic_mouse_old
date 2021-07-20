#include "shared.h"
#include <cmath>
#include <Windows.h>
#include <complex>

using namespace std;

double interWeightsRbf(float v_x, float f_n, int numARCoeff, int mdl){
	double result=0;
    for(int i=0; i<rbfNum[mdl]; i++){
        double r = (v_x - rbfCenters[mdl][i][0]) * (v_x - rbfCenters[mdl][i][0]) +
                   (f_n - rbfCenters[mdl][i][1]) * (f_n - rbfCenters[mdl][i][1]);
		// to be used kernel
        result += sqrt(r)*rbfCoeff[mdl][i + numARCoeff * rbfNum[mdl]];
	}
	return result;
}


double interPolyCoeff(float v_x, float f_n, int numARCoeff, int mdl){
	double result = 0;
    result = (1 * polyCoeff[mdl][0 + 3*numARCoeff]+
            v_x * polyCoeff[mdl][1 + 3*numARCoeff]+
            f_n * polyCoeff[mdl][2 + 3*numARCoeff]);
	return result;
} 

double interpolateRbf(float v_x, float f_n, int numARCoeff, int mdl){
    return interWeightsRbf(v_x, f_n, numARCoeff,  mdl)
            + interPolyCoeff(v_x, f_n, numARCoeff,  mdl);
}

void normInput(float &v_x, float &f_n, int mdl){
    v_x /= velNormCoeffX[mdl];
    f_n /= forceNormCoeff[mdl];
}

void ARFromLsf(float v_x, float f_n, int mdl){
	
    normInput(v_x, f_n, mdl);
    int numCoeff = nARCoeff[mdl] - 2;

    for(int i = 0; i < numCoeff; i++)
	{
        filtLSF[mdl][i] = interpolateRbf(v_x, f_n, i, mdl);
	}
	
	complex<float> pAR[MAX_COEFF]; //roots of P
	complex<float> rQ[MAX_COEFF]; //roots of Q
	complex<float> Q[MAX_COEFF+1]; 
	complex<float> P[MAX_COEFF+1];
	complex<float> Q1[MAX_COEFF+2]; //sum filter
	complex<float> P1[MAX_COEFF+2]; //difference filter
	float AR[MAX_COEFF+1];
	complex<float> rP[MAX_COEFF];

	for(int i =0; i< numCoeff; i++){

        complex<float> mycomplex (0,filtLSF[mdl][i]);
		pAR[i]=exp(mycomplex);// e^i*lsf

		if ( i % 2 == 0 ){ // separate the odd index results from the even index results
			rQ[i/2]=pAR[i];
		}
		else{
			rP[(i-1)/2]=pAR[i];
		}
	}

	//if even number of coefficients
	if ( numCoeff % 2 == 0 ){
		for (int i=numCoeff/2;i<numCoeff;i++){ 
			rQ[i]=conj(rQ[i-numCoeff/2]);//add the conjugates of the values to the end of the lists
			rP[i]=conj(rP[i-numCoeff/2]);
		}
		P[0]=1;//P and Q are vectors of 0, starting with a 1
		Q[0]=1;
		for (int i=1;i<=numCoeff;i++){
			Q[i]=0;
			P[i]=0;
		}

		//Form the polynomials P and Q, these should be real
		for (int kQ=0;kQ<numCoeff;kQ++){
			for (int i=kQ+1; i>=1;i--){
				Q[i] = Q[i]-rQ[kQ]*Q[i-1];
			}
		}
		for (int kP=0;kP<numCoeff;kP++){
			for (int i=kP+1; i>=1;i--){
				P[i] = P[i]-rP[kP]*P[i-1];
			}
		}

		float vp[2];
		vp[0]=1;
		vp[1]=-1;
		int mp=numCoeff+1;
		int np=2;
		//form difference filter by including root at z=1
		//P1 = conv(P,[1 -1])
		for (int kp=1; kp<mp+np;kp++){
			int j1p=max(1,kp+1-np);
			int j2p=min(kp,mp);
			P1[kp-1]=0;
			for (int jp=j1p; jp<=j2p;jp++){ 
				P1[kp-1]=P1[kp-1]+P[jp-1]*vp[kp-jp];
			}
		}

		float vq[2];
		vq[0]=1;
		vq[1]=1;
		int mq=mp;
		int nq=np;
		//form sum filter by including root at z=-1
		//Q1 = conv(Q,[1 1])
		for (int kq=1; kq<mq+nq;kq++){
			int j1q=max(1,kq+1-nq);
			int j2q=min(kq,mq);
			Q1[kq-1]=0;
			for (int jq=j1q; jq<=j2q;jq++){ 
				Q1[kq-1]=Q1[kq-1]+Q[jq-1]*vq[kq-jq];
			}
		}

		//Average the real values for P and Q
		for (int i=0;i<numCoeff+1;i++){
			AR[i]=(P1[i].real()+Q1[i].real())/2;
		}
	}//end even number of coefficients case

	//odd number of coefficients... same thing, but shifted
	else {
		//add the conjugates of the values to the end of the lists
		for (int i=(numCoeff+1)/2;i<numCoeff+1;i++){
			rQ[i]=conj(rQ[i-(numCoeff+1)/2]);
		}
		for (int i=(numCoeff-1)/2;i<numCoeff-1;i++){
			rP[i]=conj(rP[i-(numCoeff-1)/2]);
		}
		P[0]=1; //P and Q are vectors of 0, starting with a 1
		Q[0]=1;
		for (int i=1;i<=numCoeff+1;i++){
			Q[i]=0;
		}

		//Form the polynomials P and Q, these should be real
		for (int i=1;i<=numCoeff-1;i++){
			P[i]=0;
		}

		for (int kQ=0;kQ<numCoeff+1;kQ++){// the order in which it is looped matters
			for (int i=kQ+1; i>=1;i--){
				Q[i] = Q[i]-rQ[kQ]*Q[i-1];
			}
		}

		for (int kP=0;kP<numCoeff-1;kP++){
			for (int i=kP+1; i>=1;i--){
				P[i] = P[i]-rP[kP]*P[i-1];
			}
		}

		float v[3];
		v[0]=1;
		v[1]=0;
		v[2]=-1;
		int m=numCoeff;
		int n=3;
		//form difference filter by including root at z=+1 and z=-1
		//P1 = conv(P,[1 0 -1])
		for (int k=1; k<m+n; k++){
			int j1= max(1,k+1-n);
			int j2= min(k,m);
			P1[k-1]=0;
			for (int j=j1; j<=j2;j++){
				P1[k-1]=P1[k-1]+P[j-1]*v[k-j];
			}
		}
		//Average the real values for P and Q
		for (int i=0;i<numCoeff+1;i++){
			AR[i]=(P1[i].real()+Q[i].real())/2;
		}
	}// end of odd number of coefficients

	//Update appropriate buffer with AR coefficients
	for(int i=1; i<=numCoeff; i++){
        if (SynthesisFlag_Buffer1[mdl]){
            filtCoeff_buf2[mdl][i-1]=AR[i];
		}
		else{
            filtCoeff_buf1[mdl][i-1]=AR[i];
		}
	}

	//
    if(SynthesisFlag_Buffer1[mdl])
    {
        // interpolate the variance
        filtVariance_buf2[mdl] = abs(interpolateRbf(v_x, f_n, numCoeff + 1, mdl)); // last coeff from rbf is variance (2n from end is gain(unused))
        SynthesisFlag_Buffer1[mdl] = false;
    }

    else
    {
        // interpolate the variance
        filtVariance_buf1[mdl] = abs(interpolateRbf(v_x, f_n, numCoeff + 1, mdl)); // last coeff from rbf is variance (2n from end is gain(unused))
        SynthesisFlag_Buffer1[mdl] = true;
    }
}
