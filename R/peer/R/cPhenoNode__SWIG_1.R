cPhenoNode__SWIG_1 <-
structure(function(pheno_mean, pheno_var)
{
  ;ans = .Call('R_swig_new_cPhenoNode__SWIG_1', pheno_mean, pheno_var, PACKAGE='peer');
  class(ans) <- "_p_PEER__cPhenoNode";
  
  reg.finalizer(ans, delete_cPhenoNode)
  ans
  
}
, returnType = "_p_PEER__cPhenoNode", inputTypes = c("_p_PMatrix", 
"_p_PMatrix"), class = c("SWIGFunction", "character"))
