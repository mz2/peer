/* 
SWIG interface file for peer
- defines the interfaces visisble for SWIG
- basically sparsefa.h with some exception
Note: for some strange reason we need to make all VBFA methods public gain. 
These are all copied in this file. 
Fort this reason the interface is *not* a valid c++ header
*/


#ifndef __SPARSEFA_H__
#define __SPARSEFA_H__


#include "vbfa.h"
#include "bayesnet.h"
#include "ossolog.h"


namespace PEER
{
	int getVerbose();
	void setVerbose(int verbose);
	
	
	/*Sparse Factor analysis node for W*/
	class cWNodeSparse : public cWNode {
	protected:
		bool updateC;
	public:
		//log prior of link being on
		PMatrix pi;
		PMatrix lpi;
		PMatrix lpi_off;
		//posteior probability of link being on
		PMatrix C;
		//posterior probability of link being off (for convenience)
		PMatrix Coff;
		//diagonal component from C prior
		PMatrix CovPriorDiag;
		double tauOn;
		double tauOff;
		//public stuff is inherited from cWnode...
		cWNodeSparse(); // 
		cWNodeSparse(PMatrix E1,PMatrix pi,cBayesNet* net);
		virtual void update(cBayesNet* net);
	};
	
	class cEpsNodeSparse : public cEpsNode {
	public:
		cEpsNodeSparse(int dim, float pa, float pb, PMatrix E1): cEpsNode(dim,pa,pb,E1) {};
		virtual void update(cBayesNet* net);
	};
	
	class cSPARSEFA : public cVBFA { 
		//declare friends:
		friend class cNode;
		friend class cWNode;
		friend class cXNode;
		friend class cWNodeSparse;
		friend class cPhenoNode;
		friend class cEpsNode;
		friend class cAlphaNode;
		
	protected:
		//test: matrix with pror information about relations?
		PMatrix pi;
		double sigmaOff;
		double sigmaOn;
		virtual void init_params();
		
	public:
		
		//constructor
		cSPARSEFA();
		//destructor
		virtual ~cSPARSEFA();
		     
		//setters
		void setSigmaOff(double sigma_off) {this->sigmaOff = sigma_off;};
		//getters
		double getSigmaOff() { return this->sigmaOff;};
		
		int getNj() {return Nj;};
		int getNp() {return Np;};
		int getNk() {return Nk;};
		int getNc() {return Nc;};
		int getNmax_iterations() {return Nmax_iterations;};
		double getTolerance() {return tolerance;};
		double getVarTolerance() {return var_tolerance;}
		bool getAdd_mean() { return add_mean;}				
		double getPriorEpsA() { return Eps_pa;}
		double getPriorEpsB() { return Eps_pb;}
		double getPriorAlphaA() { return Alpha_pa;}
		double getPriorAlphaB() { return Alpha_pb;}

		//setters
		void setSparsityPrior(float64_t* matrix,int32_t rows,int32_t cols);
		void getSparsityPrior(float64_t** matrix,int32_t* rows,int32_t* cols);		
		void getZ(float64_t** matrix,int32_t* rows,int32_t* cols);


		void setNk(int Nk) {this->Nk = Nk;is_initialized=false;}
		void setAdd_mean(bool add_mean) {this->add_mean = add_mean;is_initialized=false;};
		void setNmax_iterations(int Nmax_iterations) {this->Nmax_iterations = Nmax_iterations;};
		void setTolerance(double tolerance){this->tolerance = tolerance;};
		void setVarTolerance(double tolerance) {this->var_tolerance = tolerance;};
		void setPriorAlphaA(double pa) {Alpha_pa = pa; is_initialized=false;}
		void setPriorAlphaB(double pb) {Alpha_pb = pb; is_initialized=false;}
		void setPriorEpsA(double pa) {Eps_pa = pa; is_initialized=false;}
		void setPriorEpsB(double pb) {Eps_pb = pb; is_initialized=false;}
		void setPriorAlpha(double pa,double pb){ Alpha_pa=pa;Alpha_pb = pb;is_initialized=false;}
		void setPriorEps(double pa,double pb){Eps_pa = pa;Eps_pb=pb;is_initialized=false;}
	
		//general methods:
		virtual	void init_net();	
		virtual double calcBound();
		virtual double logprob();
		virtual void update();
		
		virtual void setPhenoMean(float64_t* matrix,int32_t rows,int32_t cols);
		virtual void setPhenoVar(float64_t* matrix,int32_t rows,int32_t cols);
		virtual void setCovariates(float64_t* matrix,int32_t rows,int32_t cols);
		
		//getters
		virtual void getPhenoMean(float64_t** matrix,int32_t* rows,int32_t* cols);
		virtual void getPhenoVar(float64_t** matrix,int32_t* rows,int32_t* cols);
		virtual void getCovariates(float64_t** matrix,int32_t* rows,int32_t* cols);

		virtual void getX(float64_t** matrix,int32_t* rows,int32_t* cols);
		virtual void getW(float64_t** matrix,int32_t* rows,int32_t* cols);
		virtual void getEps(float64_t** matrix,int32_t* rows,int32_t* cols);
		virtual void getAlpha(float64_t** matrix,int32_t* rows,int32_t* cols);
		virtual void getResiduals(float64_t** matrix,int32_t* rows,int32_t* cols);
		virtual void getBounds(float64_t** matrix,int32_t* rows,int32_t* cols);
		virtual void getResidualVars(float64_t** matrix,int32_t* rows,int32_t* cols);
		
		virtual double calcBound();
		virtual double logprob();
		virtual void init_net();
		virtual void init_net_sparse();	
		virtual void update();
			    
	};// :: sparseFA
       
}


#endif
