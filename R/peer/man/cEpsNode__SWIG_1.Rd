\name{cEpsNode__SWIG_1}
\alias{cEpsNode__SWIG_1}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
%%  ~~function to do ... ~~
}
\description{
%%  ~~ A concise (1-5 lines) description of what the function does. ~~
}
\usage{
cEpsNode__SWIG_1(dim, pa, pb, E1)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{dim}{
%%     ~~Describe \code{dim} here~~
}
  \item{pa}{
%%     ~~Describe \code{pa} here~~
}
  \item{pb}{
%%     ~~Describe \code{pb} here~~
}
  \item{E1}{
%%     ~~Describe \code{E1} here~~
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
function(dim, pa, pb, E1)
{
  dim = as.integer(dim); 
  
  if(length(dim) > 1) {
    warning("using only the first element of dim");
  };
  
  
  
  
  ;ans = .Call('R_swig_new_cEpsNode__SWIG_1', dim, pa, pb, E1, PACKAGE='peer');
  class(ans) <- "_p_PEER__cEpsNode";
  
  reg.finalizer(ans, delete_cEpsNode)
  ans
  
  }
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ ~kwd1 }
\keyword{ ~kwd2 }% __ONLY ONE__ keyword per line
