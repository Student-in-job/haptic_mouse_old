/***********************************************************************************************************************************
COPYRIGHT AND PERMISSION NOTICE
Penn Software The Penn Haptic Texture Toolkit
Copyright (C) 2013 The Trustees of the University of Pennsylvania
All rights reserved.

See copyright and permission notice at the end of this file.

Report bugs to Heather Culbertson (hculb@seas.upenn.edu, +1 215-573-6748) or Katherine J. Kuchenbecker (kuchenbe@seas.upenn.edu, +1 215-573-2786)

This code is based on the original TexturePad haptic rendering system designed by Joseph M. Romano.
************************************************************************************************************************************/


/*shared memory file
*/
#ifndef _SHARED_H_
#define _SHARED_H_

#define SAMPLE_RATE (1000)
#define MAX_COEFF 25
#define MAX_MACOEFF 25
#define NUM_TEX 100
#define IM_SIZE 3145728
#define MAX_NUM_RBF 4000
#include <string>
#include <vector>

// Directional coeff here
extern float featMax[200][3];	//features maximums for (v_x, v_y, f_n)
extern float featMin[200][3];	//features minimums for (v_x, v_y, f_n)
extern int rbfNum[200];			//number of RBF coefficients
extern int nARCoeff[200];		//number of AR coefficients
extern double rbfCoeff[200][MAX_NUM_RBF];   //RBF coefficients
extern double polyCoeff[200][80];			//Polynomial coefficients
extern float rbfCenters[200][MAX_NUM_RBF][3];
extern float velNormCoeffX[200];
extern float velNormCoeffY[200];
extern float forceNormCoeff[200];
extern float gainConst[200];

extern int coeffNum[200];
extern int MAcoeffNum;
extern float filtLSF[200][MAX_COEFF];
extern float filtMALSF[200][MAX_MACOEFF];
extern bool SynthesisFlag_Buffer1[200];
extern bool isARMA;
extern float filtCoeff_buf1[200][MAX_COEFF];
extern float filtCoeff_buf2[200][MAX_COEFF];
extern float filtMACoeff_buf1[200][MAX_MACOEFF];
extern float filtMACoeff_buf2[200][MAX_MACOEFF];
extern float filtVariance_buf1[200];
extern float filtVariance_buf2[200];
extern float filtGain_buf1[200];
extern float filtGain_buf2[200];
extern float mu_k[200];

extern int textNum;
extern char texArray[NUM_TEX][50]; // stores names of textures
extern char imArray[NUM_TEX][100]; // stores filepaths to texture images
extern unsigned char imData[IM_SIZE];

#endif

/***********************************************************************************************************************************
COPYRIGHT AND PERMISSION NOTICE
Penn Software The Penn Haptic Texture Toolkit
Copyright (C) 2013 The Trustees of the University of Pennsylvania
All rights reserved.

The Trustees of the University of Pennsylvania (“Penn”) and Heather Culbertson, Juan Jose Lopez Delgado, and Katherine J. Kuchenbecker, the developer (“Developer”) of Penn Software The Penn Haptic Texture Toolkit (“Software”) give recipient (“Recipient”) and Recipient’s Institution (“Institution”) permission to use, copy, and modify the software in source and binary forms, with or without modification for non-profit research purposes only provided that the following conditions are met:

1)	All copies of Software in binary form and/or source code, related documentation and/or other materials provided with the Software must reproduce and retain the above copyright notice, this list of conditions and the following disclaimer.

2)	Recipient shall have the right to create modifications of the Software (“Modifications”) for their internal research and academic purposes only. 

3)	All copies of Modifications in binary form and/or source code and related documentation must reproduce and retain the above copyright notice, this list of conditions and the following disclaimer.

4)	Recipient and Institution shall not distribute Software or Modifications to any third parties without the prior written approval of Penn.

5)	Recipient will provide the Developer with feedback on the use of the Software and Modifications, if any, in their research.  The Developers and Penn are permitted to use any information Recipient provides in making changes to the Software. All feedback, bug reports and technical questions shall be sent to: 

Heather Culbertson, hculb@seas.upenn.edu, +1 215-573-6748
Katherine J. Kuchenbecker, kuchenbe@seas.upenn.edu, +1 215-573-2786

6)	Recipient acknowledges that the Developers, Penn and its licensees may develop modifications to Software that may be substantially similar to Recipient’s modifications of Software, and that the Developers, Penn and its licensees shall not be constrained in any way by Recipient in Penn’s or its licensees’ use or management of such modifications. Recipient acknowledges the right of the Developers and Penn to prepare and publish modifications to Software that may be substantially similar or functionally equivalent to your modifications and improvements, and if Recipient or Institution obtains patent protection for any modification or improvement to Software, Recipient and Institution agree not to allege or enjoin infringement of their patent by the Developers, Penn or any of Penn’s licensees obtaining modifications or improvements to Software from the Penn or the Developers.

7)	Recipient and Developer will acknowledge in their respective publications the contributions made to each other’s research involving or based on the Software. The current citations for Software are:

Heather Culbertson, Juan Jose Lopez Delgado, and Katherine J. Kuchenbecker. One Hundred Data-Driven Haptic Texture Models and Open-Source Methods for Rendering on 3D Objects. In Proc. IEEE Haptics Symposium, February 2014.

8)	Any party desiring a license to use the Software and/or Modifications for commercial purposes shall contact The Center for Technology Transfer at Penn at 215-898-9591.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS, CONTRIBUTORS, AND THE TRUSTEES OF THE UNIVERSITY OF PENNSYLVANIA "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER, CONTRIBUTORS OR THE TRUSTEES OF THE UNIVERSITY OF PENNSYLVANIA BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

************************************************************************************************************************************/
