\name{defineEnumeration}
\alias{defineEnumeration}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
%%  ~~function to do ... ~~
}
\description{
%%  ~~ A concise (1-5 lines) description of what the function does. ~~
}
\usage{
defineEnumeration(name, .values, where = topenv(parent.frame()), suffix = "Value")
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{name}{
%%     ~~Describe \code{name} here~~
}
  \item{.values}{
%%     ~~Describe \code{.values} here~~
}
  \item{where}{
%%     ~~Describe \code{where} here~~
}
  \item{suffix}{
%%     ~~Describe \code{suffix} here~~
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
function(name, .values, where = topenv(parent.frame()), suffix = "Value")
{
   # Mirror the class definitions via the E analogous to .__C__
  defName = paste(".__E__", name, sep = "")
  assign(defName,  .values,  envir = where)

  if(nchar(suffix))
    name = paste(name, suffix, sep = "")

  setClass(name, contains = "EnumerationValue", where = where)
  }
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ ~kwd1 }
\keyword{ ~kwd2 }% __ONLY ONE__ keyword per line
