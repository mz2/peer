cXNode_calcBound <-
structure(function(self, net, .copy = FALSE)
{
  ;.Call('R_swig_cXNode_calcBound', self, net, as.logical(.copy), PACKAGE='peer');
  
}
, returnType = "numeric", inputTypes = c("_p_PEER__cXNode", "_p_cBayesNet"
), class = c("SWIGFunction", "character"))
