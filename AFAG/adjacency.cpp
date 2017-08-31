#include "adjacency.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bl_enum.hxx>

// ACIS headers
#include "get_top.hxx"
#include "cstrapi.hxx"
#include "intrapi.hxx"
#include "kernapi.hxx"

using namespace std;

int get_face_index(FACE * f, ENTITY_LIST face_list) {
	FACE * temp_face;
	int i = 0;
	while (temp_face = (FACE*)face_list.next()) {
		if (f == temp_face) {
			return i;
		}
		i++;
	}

	cout << "ERROR: The face is not in the facelist. \n";
	return i;
}

// Output:	1. no. of faces, edges, convex edges, concave edges
//			2. adjacency links w/ convexity 

int adjacency_links(ENTITY_LIST ents_in){

	// Open txt file
	std::ofstream ofs;
	ofs.open("REPORT.txt");

	// Get counts for faces and edges

	ENTITY_LIST face_list;
	ENTITY_LIST edge_list;

	BODY * my_body = NULL;
	my_body = (BODY*)ents_in[0];
	
	get_faces(my_body, face_list);
	get_edges(my_body, edge_list);

	int num_faces = face_list.count(); 
	int num_edges = edge_list.count();

	ofs << "Edge count: " << num_edges << endl;
	ofs << "Face count: " << num_faces << endl;

	// Find faces that share an edge
	
	FACE * temp_face;
	EDGE * temp_edge;
	ENTITY_LIST temp_face_list;
	vector <int> face_indices;
	int index;
	int cvty_value;
	vector <int> adjacency_matrix(num_faces*num_faces, 0);	
	bl_ed_convexity cvty;
	ENTITY_LIST convex_edge_list;
	ENTITY_LIST concave_edge_list;
	
	edge_list.init();

	// For each edge find the faces that share an edge and determine convexity

	ofs << "\nFace Adjacency Matrix (1 is convex, -1 is concave): \n";
	while (temp_edge = (EDGE*)edge_list.next()) {

		// For current edge, find faces that share the edge
		api_get_faces(temp_edge, temp_face_list);
		temp_face_list.init();
		while (temp_face = (FACE*)(temp_face_list.next())) {
			index = get_face_index(temp_face, face_list);
			face_indices.push_back(index);
		}
		temp_face_list.clear();

		// Determine Convexities 
		api_edge_convexity_param(temp_edge, 0, cvty, NULL);
		if (cvty == bl_ed_convex || cvty == bl_ed_convex_smooth) {
			cvty_value = 1;
			convex_edge_list.add(temp_edge);
		}
		else if (cvty == bl_ed_concave || cvty == bl_ed_concave_smooth) {
			cvty_value = -1;
			concave_edge_list.add(temp_edge);
		}

		// Add convexities to face adjacency matrix
		adjacency_matrix[face_indices[0] + (num_faces) * face_indices[1]] = cvty_value;
		adjacency_matrix[face_indices[1] + (num_faces) * face_indices[0]] = cvty_value;
		
		face_indices.clear();
	}

	// Write Adjacency Matrix to file

	for (int i = 0; i < num_faces; i++) {
		for (int k = 0; k < num_faces; k++) {
			ofs << adjacency_matrix[k + (num_faces)*i] << " ";
		}ofs << endl;
	}

	// Print Concavity/convexity counts

	ofs << "\nConvex edge count: " << convex_edge_list.count() << endl;
	ofs << "Concave edge count: " << concave_edge_list.count() << endl;

	ofs.close();

	return 0;
};

