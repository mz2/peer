\name{PEER_setNmax_iterations}
\alias{PEER_setNmax_iterations}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
%%  ~~function to do ... ~~
}
\description{
%%  ~~ A concise (1-5 lines) description of what the function does. ~~
}
\usage{
PEER_setNmax_iterations(self, Nmax_iterations)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{self}{
%%     ~~Describe \code{self} here~~
}
  \item{Nmax_iterations}{
%%     ~~Describe \code{Nmax_iterations} here~~
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
function(self, Nmax_iterations)
{
  Nmax_iterations = as.integer(Nmax_iterations); 
  
  if(length(Nmax_iterations) > 1) {
    warning("using only the first element of Nmax_iterations");
  };
  
  ;.Call('R_swig_PEER_setNmax_iterations', self, Nmax_iterations, PACKAGE='peer');
  
  }
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ ~kwd1 }
\keyword{ ~kwd2 }% __ONLY ONE__ keyword per line
