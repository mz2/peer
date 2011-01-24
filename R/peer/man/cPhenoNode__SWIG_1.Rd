\name{cPhenoNode__SWIG_1}
\alias{cPhenoNode__SWIG_1}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
%%  ~~function to do ... ~~
}
\description{
%%  ~~ A concise (1-5 lines) description of what the function does. ~~
}
\usage{
cPhenoNode__SWIG_1(pheno_mean, pheno_var)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{pheno_mean}{
%%     ~~Describe \code{pheno_mean} here~~
}
  \item{pheno_var}{
%%     ~~Describe \code{pheno_var} here~~
}
}
\details{
%%  ~~ If necessary, more details than the description above ~~
}
\value{
%%  ~Describe the value returned
%%  If it is a LIST, use
%%  \item{comp1 }{Description of 'comp1'}
%%  \item{comp2 }{Description of 'comp2'}
%% ...
}
\references{
%% ~put references to the literature/web site here ~
}
\author{
%%  ~~who you are~~
}
\note{
%%  ~~further notes~~
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
%% ~~objects to See Also as \code{\link{help}}, ~~~
}
\examples{
##---- Should be DIRECTLY executable !! ----
##-- ==>  Define data, use random,
##--	or do  help(data=index)  for the standard data sets.

## The function is currently defined as
function(pheno_mean, pheno_var)
{
  ;ans = .Call('R_swig_new_cPhenoNode__SWIG_1', pheno_mean, pheno_var, PACKAGE='peer');
  class(ans) <- "_p_PEER__cPhenoNode";
  
  reg.finalizer(ans, delete_cPhenoNode)
  ans
  
  }
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ ~kwd1 }
\keyword{ ~kwd2 }% __ONLY ONE__ keyword per line
