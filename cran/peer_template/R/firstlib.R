.First.lib <- function(lib, pkg) {
  library.dynam("peer", pkg, lib)
}
