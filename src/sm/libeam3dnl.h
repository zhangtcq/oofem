/* $Header: /home/cvs/bp/oofem/sm/src/libeam3dnl.h,v 1.6 2003/04/06 14:08:30 bp Exp $ */
/*
 *
 *                 #####    #####   ######  ######  ###   ###
 *               ##   ##  ##   ##  ##      ##      ## ### ##
 *              ##   ##  ##   ##  ####    ####    ##  #  ##
 *             ##   ##  ##   ##  ##      ##      ##     ##
 *            ##   ##  ##   ##  ##      ##      ##     ##
 *            #####    #####   ##      ######  ##     ##
 *
 *
 *             OOFEM : Object Oriented Finite Element Code
 *
 *               Copyright (C) 1993 - 2008   Borek Patzak
 *
 *
 *
 *       Czech Technical University, Faculty of Civil Engineering,
 *   Department of Structural Mechanics, 166 29 Prague, Czech Republic
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

//   ************************
//   *** CLASS LIBeam3dNL ***
//   ************************

#ifndef libeam3dnl_h
#define libeam3dnl_h

#include "nlstructuralelement.h"
#include "gausspnt.h"

namespace oofem {
class LIBeam3dNL : public NLStructuralElement
{
    /*
     * This class implements a 3-dimensional Linear Isoparametric
     * Mindlin theory beam element, with reduced integration.
     * Geometric nonlinearities are taken into account.
     * Based on Element due to Simo and Vu-Quoc, description taken from
     * Crisfield monograph.
     */
private:
    /// initial length
    double l0;
    /// last equlibrium triad at the centre
    FloatMatrix tc;
    // curvature at the centre
    // FloatArray  kappa;
    /// temporary triad at the centre
    FloatMatrix tempTc;
    /// time stamp of temporary centre triad
    StateCounterType tempTcCounter;
    /// reference node
    int referenceNode;
public:

    LIBeam3dNL(int, Domain *);                       // constructor
    ~LIBeam3dNL()  { }                               // destructor

    // FloatMatrix*  ComputeConstitutiveMatrixAt (GaussPoint*) ;
    // FloatArray*   ComputeResultingBodyForceAt (TimeStep*) ;
    void          computeLumpedMassMatrix(FloatMatrix &answer, TimeStep *tStep);
    void          computeConsistentMassMatrix(FloatMatrix &answer, TimeStep *tStep, double &mass)
    { computeLumpedMassMatrix(answer, tStep); }
    void          computeStrainVector(FloatArray &answer, GaussPoint *gp, TimeStep *tStep);
    //  int           computeGtoLRotationMatrix (FloatMatrix&);  // giveRotationMatrix () ;
    //  void          computeInitialStressMatrix (FloatMatrix& answer, TimeStep* tStep) ;

    virtual int            computeNumberOfDofs(EquationID ut) { return 12; }
    virtual void giveDofManDofIDMask(int inode, EquationID, IntArray &) const;
    double        computeVolumeAround(GaussPoint *);
    /**
     * Computes the global coordinates from given element's local coordinates.
     * @returns nonzero if successful
     */
    virtual int computeGlobalCoordinates(FloatArray &answer, const FloatArray &lcoords);

    //
    // definition & identification
    //
    const char *giveClassName() const { return "LIBeam3dNL"; }
    classType             giveClassID()          const { return LIBeam3dNLClass; }
    IRResultType initializeFrom(InputRecord *ir);

#ifdef __OOFEG
    void          drawRawGeometry(oofegGraphicContext &);
    void drawDeformedGeometry(oofegGraphicContext &, UnknownType);
#endif


    virtual void          computeStiffnessMatrix(FloatMatrix &answer,
                                                 MatResponseMode rMode, TimeStep *tStep);
    virtual void giveInternalForcesVector(FloatArray &answer,
                                          TimeStep *, int useUpdatedGpRecord = 0);

    integrationDomain  giveIntegrationDomain() { return _Line; }
    MaterialMode          giveMaterialMode()  { return _3dBeam; }

protected:
    // edge load support
    void  computeEgdeNMatrixAt(FloatMatrix &answer, GaussPoint *);
    void  giveEdgeDofMapping(IntArray &answer, int) const;
    double        computeEdgeVolumeAround(GaussPoint *, int);
    void          computeEdgeIpGlobalCoords(FloatArray &answer, GaussPoint *gp, int iEdge)
    { computeGlobalCoordinates( answer, * ( gp->giveCoordinates() ) ); }
    int   computeLoadLEToLRotationMatrix(FloatMatrix &, int, GaussPoint *);
    int  computeLoadGToLRotationMtrx(FloatMatrix &answer);
    void computeBodyLoadVectorAt(FloatArray &answer, Load *load, TimeStep *tStep, ValueModeType mode);

    void  updateYourself(TimeStep *tStep);
    void  initForNewStep();
    //void          computeTemperatureStrainVectorAt (FloatArray& answer, GaussPoint*, TimeStep*, ValueModeType mode);
    void          computeBmatrixAt(GaussPoint *, FloatMatrix &, int, int)
    { _error("computeBmatrixAt: not implemented"); }
    //int           computeGtoLRotationMatrix (FloatMatrix& answer);

    // nonlinear part of geometrical eqs. for i-th component of strain vector.
    // void          computeNLBMatrixAt (FloatMatrix& answer, GaussPoint*, int ) ;
    void          computeNmatrixAt(GaussPoint *, FloatMatrix &);
    void          computeGaussPoints();
    double        giveLength();
    //  double        givePitch () ;
    int           giveLocalCoordinateSystem(FloatMatrix &answer);
    /**
     * Updates the temporary triad at the centre to the state identified by given solution step.
     * The attribute tempTc is changed to reflect new state and tempTcCounter is set to
     * solution step conter to avoid multiple updates.
     * @param tStep solution step identifying reached state
     */
    void          updateTempTriad(TimeStep *tStep);
    /**
     * compute the temporary curvature at the centre to the state identified by given solution step.
     * @param tStep solution step identifying reached state
     */
    void          computeTempCurv(FloatArray &answer, TimeStep *tStep);

    /**
     * Evaluates the S matrix from given vector vec.
     * @param answer assembled result
     * @param vec source vector
     */
    void          computeSMtrx(FloatMatrix &answer, FloatArray &vec);
    /**
     * Evaluates the rotation matrix for large rotations according to Rodrigues formula for given
     * pseudovector psi.
     * @param answer result
     * @param psi pseudovector
     */
    void          computeRotMtrx(FloatMatrix &answer, FloatArray &psi);
    /**
     * Computes X mtrx at given solution state.
     * @param answer returned x matrix.
     * @param tStep determines solution state.
     */
    void          computeXMtrx(FloatMatrix &answer, TimeStep *tStep);
    /**
     * Computes x_21' vector for given solution state.
     * @param answer returned x_21'
     * @param tStep determines solution state.
     */
    void          computeXdVector(FloatArray &answer, TimeStep *tStep);
};
} // end namespace oofem
#endif // libeam3dnl_h
