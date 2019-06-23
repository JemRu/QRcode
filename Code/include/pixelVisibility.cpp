#include "pixelVisibility.h"
// Define the used kernel and arrangement  
typedef CGAL::Exact_predicates_exact_constructions_kernel       Kernel;
typedef Kernel::FT												FT;
typedef Kernel::Point_2											Point_2;
typedef Kernel::Segment_2                                       Segment_2;
typedef CGAL::Arr_segment_traits_2<Kernel>                      Traits_2;
typedef CGAL::Arrangement_2<Traits_2>                           Arrangement_2;
typedef Arrangement_2::Halfedge_const_handle                    Halfedge_const_handle;
typedef Arrangement_2::Face_handle                              Face_handle;
// Define the used visibility class 
typedef CGAL::Triangular_expansion_visibility_2<Arrangement_2, CGAL::Tag_true>  TEV;

 std::vector<cv::Point> conn::pixelVisibility(cv::Point &queryPixel, std::vector<std::vector<cv::Point>> contour){
	std::vector<cv::Point> visibleContour;
	 
	std::vector<Segment_2> segments;
	for (int i = 0; i <contour.size(); i++) {
		for (int j = 0; j < contour[i].size(); j++)
		{
			Point_2 s(static_cast<double>(contour[i][j].x), static_cast<double>(contour[i][j].y));
			Point_2 d(static_cast<double>(contour[i][(j + 1) % contour[i].size()].x), static_cast<double>(contour[i][(j + 1) % contour[i].size()].y));
			segments.push_back(Segment_2(s, d));
			//std::cout << "S: ( " << s.x() << " , " << s.y() << " ) D: ( " << d.x() << " , " << d.y() << " )" << std::endl;
		}
		//std::cout << "contours : " << i << std::endl;
	}
	//Defining the query point
	Point_2 q(static_cast<double> (queryPixel.x) , static_cast<double> (queryPixel.y));
	// insert geometry into the arrangement 
	Arrangement_2 env;
	CGAL::insert_non_intersecting_curves(env, segments.begin(), segments.end());

	//Find the halfedge whose target is the query point.
	//(usually you may know that already by other means)  
	Face_handle fit;
	for (fit = env.faces_begin(); fit != env.faces_end(); ++fit) {
		if (!fit->is_unbounded()) {
			break;
			std::cout << "Holy shit" << std::endl;
		}
	}
	//visibility query
	Arrangement_2 output_arr;
	TEV tev(env);
	Face_handle fh = tev.compute_visibility(q, fit, output_arr);
	Arrangement_2::Ccb_halfedge_circulator curr = fh->outer_ccb();

	visibleContour.emplace_back(static_cast<int>(CGAL::to_double(curr->source()->point().x())), static_cast<int>(CGAL::to_double(curr->source()->point().y())));
	while (++curr != fh->outer_ccb()) {
		visibleContour.emplace_back(static_cast<int>(CGAL::to_double(curr->source()->point().x())), static_cast<int>(CGAL::to_double(curr->source()->point().y())));
	}

	//Point_2 p1(1, 2), p2(12, 3), p3(19, -2), p4(12, 6), p5(14, 14), p6(9, 5), p7(5, 10);
	// Point_2 h1(8, 3), h2(10, 3), h3(8, 4), h4(10, 6), h5(11, 6), h6(11, 7);
	// std::vector<Segment_2> segments;
	// segments.push_back(Segment_2(p1, p2));
	// segments.push_back(Segment_2(p2, p3));
	// segments.push_back(Segment_2(p3, p4));
	// segments.push_back(Segment_2(p4, p5));
	// segments.push_back(Segment_2(p5, p6));
	// segments.push_back(Segment_2(p6, p7));
	// segments.push_back(Segment_2(p7, p1));

	// segments.push_back(Segment_2(h1, h2));
	// segments.push_back(Segment_2(h2, h3));
	// segments.push_back(Segment_2(h3, h1));
	// segments.push_back(Segment_2(h4, h5));
	// segments.push_back(Segment_2(h5, h6));
	// segments.push_back(Segment_2(h6, h4));

	// // insert geometry into the arrangement 
	// Arrangement_2 env;
	// CGAL::insert_non_intersecting_curves(env, segments.begin(), segments.end());

	// //Find the halfedge whose target is the query point.
	// //(usually you may know that already by other means)  
	// Point_2 query_point = p4;
	// Halfedge_const_handle he = env.halfedges_begin();
	// while (he->source()->point() != p3 || he->target()->point() != p4)
	//	 he++;

	// //visibility query
	// Arrangement_2 output_arr;
	// TEV tev(env);
	// Face_handle fh = tev.compute_visibility(query_point, he, output_arr);

	// //print out the visibility region.
	// std::cout << "Regularized visibility region of q has "
	//	 << output_arr.number_of_edges()
	//	 << " edges." << std::endl;

	// std::cout << "Boundary edges of the visibility region:" << std::endl;
	// Arrangement_2::Ccb_halfedge_circulator curr = fh->outer_ccb();
	// std::cout << "[" << curr->source()->point() << " -> " << curr->target()->point() << "]" << std::endl;
	// while (++curr != fh->outer_ccb())
	//	 std::cout << "[" << curr->source()->point() << " -> " << curr->target()->point() << "]" << std::endl;

	return visibleContour;
 }
