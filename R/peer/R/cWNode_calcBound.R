cWNode_calcBound <-
structure(function(self, net, .copy = FALSE)
{
  ;.Call('R_swig_cWNode_calcBound', self, net, as.logical(.copy), PACKAGE='peer');
  
}
, returnType = "numeric", inputTypes = c("_p_PEER__cWNode", "_p_cBayesNet"
), class = c("SWIGFunction", "character"))
