#ifndef Resistive_h
#define Resistive_h
#include "Common/Common.h"
#include "Geometry/Geometry.h"
#include "Readout/Readout.h"
#include "Common/Position.h"
#include "Common/Secondary.h"
#include <cmath>
#include <iostream>
#include <fstream>
using namespace std;
// XYZ are refered to the detector frame where the anode is on the XY plane and the electrinc drift field is along the Z direction
namespace PARSIFAL2{
  class Resistive{
  public:
    //Constructur                                                                                                                                                                                      
    Resistive(int setup, Geometry* geo, Readout* read);
    // Destructor
    ~Resistive() { delete r; };
    //Function                                                                                                                                                                                         
    bool               Get_PrintInfo     () {return PrintInfo;};
    bool               Get_PrintNTuple   () {return PrintNTuple;};
    Position*          Get_FinalPosition (Position* pos_init);
    vector<Secondary*> Diffusion         (vector<Secondary*> pre_resistive);
    void               Resist            ();
    void               Set_TauResist     (double io){tau = io;};
    void               Set_Sigma0Resist  (double io){sigma0 = io;};

    //Function Get
    double    Get_Diff_X        (double x);
    double    Get_Diff_Y        (double y);
    double    Get_Diff_T        (double t);
  private:
    //Variables
    bool      PrintInfo;
    bool      PrintNTuple;
    int       Setup;
    Geometry *geometry;
    Readout  *readout;
    TRandom3 *r;
    double sigma_res_diff;
    double sigma0;
    double tau;
    bool skip_resistive;
    // Temporal diffusion width sigma_res(t, t0) [mm]
    double sigma_res(double t, double t0) const {
      const double dt = t - t0;
      if(dt <= 0.0) return 0.0;
      if(Resistive_Model == 2){
        // Morello linear spreading
        return sigma0 * (1.0 + dt / tau);
      }
      if(Resistive_Model == 6 || Resistive_Model == 7){
        // Pure Dixit 2D RC dispersion (Dixit & Rankin 2006, Eq. 2-3): sigma(t) = sqrt(2 * h * dt)
        return std::sqrt(2.0 * sigma0 * sigma0 * dt / tau);
      }
      // T2K+shepherd resistive diffusion (default for STD, Djunes, Exp, Lorentz, 2D-PRF)
      const double t_ion_tail = 50.0;
      if(dt < t_ion_tail) return std::sqrt((2.0 / 3.0) * sigma0 * sigma0 * dt / tau);
      return std::sqrt((2.0 / 3.0) * sigma0 * sigma0 * (2.0 * dt + std::sqrt(dt * (dt - t_ion_tail)) - t_ion_tail) / tau);
    }

    // Exact odd spatial cumulative integral F_spatial(r, s) = int_0^r rho(x; s) dx in [-0.5, +0.5].
    // Guarantees left-right symmetry F(-r) = -F(r) and exact charge conservation F(+inf) - F(-inf) = 1.
    double F_spatial(double r, double s) const {
      if(Resistive_Model == 6){
        // Extended Ramo-Shockley: Prompt electrostatic image (Lorentzian, HWHM d = sigma0)
        // convolved with Delayed Dixit 2D RC dispersion (Gaussian, std = s) -> exact Voigt CDF
        // evaluated via symmetric 12-node Gauss-Hermite quadrature (exact odd symmetry & charge conservation).
        const double d = sigma0;
        if(d <= 0.0) return (r > 0.0) ? 0.5 : ((r < 0.0) ? -0.5 : 0.0);
        if(s <= 1e-12) return std::atan(r / d) / TMath::Pi();
        static constexpr double z_pos[6] = {
          4.444030019441390e-01, 1.340375197151617e+00, 2.259464451000799e+00,
          3.223709828770097e+00, 4.271825847932281e+00, 5.500901704467748e+00
        };
        static constexpr double w_pos[6] = {
          3.216643615128300e-01, 1.469670480453300e-01, 2.911668791236418e-02,
          2.203380687533198e-03, 4.837184922590633e-05, 1.499927167637169e-07
        };
        double sum = 0.0;
        for(int k = 0; k < 6; k++){
          const double sz = s * z_pos[k];
          sum += w_pos[k] * (std::atan((r - sz) / d) + std::atan((r + sz) / d));
        }
        return sum / TMath::Pi();
      }
      if(Resistive_Model == 7){
        // Two-scale Dixit 2D RC dispersion with prompt electrostatic width d = sigma0:
        //   slow local RC mode (weight 1 - alpha): var = d^2 + s^2
        //   fast capacitive/inter-strip RC mode (weight alpha): var = d^2 + kappa * s^2
        // Closed-form, strictly monotonic on strip 0, exact odd symmetry and charge conservation.
        const double d = sigma0;
        if(d <= 0.0 && s <= 0.0) return (r > 0.0) ? 0.5 : ((r < 0.0) ? -0.5 : 0.0);
        constexpr double alpha = 0.30;
        constexpr double kappa = 20.0;
        const double s1 = std::sqrt(d * d + s * s);
        const double s2 = std::sqrt(d * d + kappa * s * s);
        return (1.0 - alpha) * 0.5 * TMath::Erf(r / (TMath::Sqrt2() * s1))
             + alpha         * 0.5 * TMath::Erf(r / (TMath::Sqrt2() * s2));
      }
      if(s <= 0.0) return (r > 0.0) ? 0.5 : ((r < 0.0) ? -0.5 : 0.0);
      if(Resistive_Model == 1){
        // Djunes Janssens (DRD1 2024-01-31, slide 14):
        //   rho(r; s) = [ |r|*(erf(|r|/(sqrt(2)*s)) - 1) + sqrt(2/pi)*s*exp(-r^2/(2*s^2)) ] / s^2
        const double sgn = (r >= 0.0) ? 1.0 : -1.0;
        const double u = std::fabs(r) / (TMath::Sqrt2() * s);
        // Guard against overflow in u*u*erfc(u) when exp(-u^2) underflows to 0
        if(u > 26.0) return sgn * 0.5;
        const double val = 0.5 * TMath::Erf(u)
                         - u * u * TMath::Erfc(u)
                         + (u / std::sqrt(TMath::Pi())) * std::exp(-u * u);
        return sgn * val;
      }
      if(Resistive_Model == 3){
        // Exponential / Laplace profile (RMS = s): rho(r; s) = (1/(sqrt(2)*s)) * exp(-sqrt(2)*|r|/s)
        const double sgn = (r >= 0.0) ? 1.0 : -1.0;
        return sgn * 0.5 * (1.0 - std::exp(-TMath::Sqrt2() * std::fabs(r) / s));
      }
      if(Resistive_Model == 4){
        // Lorentzian / Cauchy profile: rho(r; s) = (1/pi) * s / (r^2 + s^2)
        return std::atan(r / s) / TMath::Pi();
      }
      if(Resistive_Model == 5){
        // 2D electrostatic Pad Response Function (1/r^3 tail): rho(r; s) = s^2 / (2*(r^2 + s^2)^(3/2))
        return 0.5 * r / std::sqrt(r * r + s * s);
      }
      // STD Gaussian spatial profile (used by STD and Morello)
      return 0.5 * TMath::Erf(r / (TMath::Sqrt2() * s));
    }

    // Strip-integrated charge on strip_step at time t (uniform across all resistive models)
    double Q_t(double pitch, int strip_step, double t0, double t, double q) const {
      if(t < t0) return 0.0;
      const double xL = -0.5 * pitch + strip_step * pitch;
      const double xR =  0.5 * pitch + strip_step * pitch;
      if(t == t0){
        // Models 6 and 7 include prompt electrostatic induction at t == t0 (width sigma0)
        if(Resistive_Model == 6 || Resistive_Model == 7) return q * (F_spatial(xR, 0.0) - F_spatial(xL, 0.0));
        // Other models have zero width at t == t0: all charge sits on the central strip
        return (strip_step == 0) ? q : 0.0;
      }
      const double sr = sigma_res(t, t0);
      return q * (F_spatial(xR, sr) - F_spatial(xL, sr));
    }

    
    //Function Set                                                                                                                                                                                     
    void      Set_PrintInfo     (bool io) {PrintInfo=io;};
    void      Set_PrintNTuple   (bool io) {PrintNTuple=io;};
    void      Set_Setup         (int io)  {Setup=io;};
    //Function Get                                                                                                                                                                                     
    double    Get_Setup         ()        {return Setup;}
    //double    Get_Diff_X        (double x);
    //double    Get_Diff_Y        (double y);
    double    Get_Diff_Z        (double z);
  };
}
#endif


