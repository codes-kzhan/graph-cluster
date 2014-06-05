
all: stand-alone

clean:
	rm -rf *.o
	rm -rf *.oct
	rm -rf *.mex

.PHONY: stand-alone octave-bindings matlab-bindings

# Dependencies
HEADERS = lso_cluster.hpp lso_cluster_impl.hpp loss_functions.hpp trace_file_io.cpp argument_parser.hpp argument_parser_cli.hpp lso_argument_parser.hpp util.hpp sparse_matrix.hpp
CXXFLAGS = -O2 -Wall

# Stand alone program
stand-alone: lso_cluster nmf_cluster
lso_cluster: lso_cluster.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ lso_cluster.cpp
nmf_cluster: nmf_cluster.cpp $(HEADERS) nmf_cluster.hpp
	$(CXX) $(CXXFLAGS) -o $@ nmf_cluster.cpp

# Octave bindings
octave-bindings: lso_cluster.oct
lso_cluster.oct: lso_cluster.cpp $(HEADERS)

# Matlab bindings
matlab-bindings: lso_cluster.mex
lso_cluster.mex: lso_cluster.mex.cpp $(HEADERS)
