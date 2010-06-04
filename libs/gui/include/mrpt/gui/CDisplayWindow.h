/* +---------------------------------------------------------------------------+
   |          The Mobile Robot Programming Toolkit (MRPT) C++ library          |
   |                                                                           |
   |                   http://mrpt.sourceforge.net/                            |
   |                                                                           |
   |   Copyright (C) 2005-2010  University of Malaga                           |
   |                                                                           |
   |    This software was written by the Machine Perception and Intelligent    |
   |      Robotics Lab, University of Malaga (Spain).                          |
   |    Contact: Jose-Luis Blanco  <jlblanco@ctima.uma.es>                     |
   |                                                                           |
   |  This file is part of the MRPT project.                                   |
   |                                                                           |
   |     MRPT is free software: you can redistribute it and/or modify          |
   |     it under the terms of the GNU General Public License as published by  |
   |     the Free Software Foundation, either version 3 of the License, or     |
   |     (at your option) any later version.                                   |
   |                                                                           |
   |   MRPT is distributed in the hope that it will be useful,                 |
   |     but WITHOUT ANY WARRANTY; without even the implied warranty of        |
   |     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         |
   |     GNU General Public License for more details.                          |
   |                                                                           |
   |     You should have received a copy of the GNU General Public License     |
   |     along with MRPT.  If not, see <http://www.gnu.org/licenses/>.         |
   |                                                                           |
   +---------------------------------------------------------------------------+ */
#ifndef  CDisplayWindow_H
#define  CDisplayWindow_H

#include <mrpt/gui/CBaseGUIWindow.h>
#include <mrpt/utils/CImage.h>

namespace mrpt
{
	/** Classes for creating GUI windows for 2D and 3D visualization. */
	namespace gui
	{
		using namespace mrpt::utils;

		DEFINE_MRPT_OBJECT_PRE_CUSTOM_BASE_LINKAGE(CDisplayWindow, mrpt::gui::CBaseGUIWindow, GUI_IMPEXP)

		/** This class creates a window as a graphical user interface (GUI) for displaying images to the user.
		 *
		 *  For a list of supported events with the observer/observable pattern, see the discussion in mrpt::gui::CBaseGUIWindow.
		 *
		 */
		class GUI_IMPEXP CDisplayWindow : public mrpt::gui::CBaseGUIWindow
		{
			// This must be added to any CSerializable derived class:
			DEFINE_MRPT_OBJECT( CDisplayWindow )

		protected:

			/** Enables or disables the visualization of cursor coordinates on the window caption.
			  */
			bool			m_enableCursorCoordinates;

		public:
			/** Constructor
			 */
			CDisplayWindow( const std::string &windowCaption = std::string(), unsigned int initWidth = 400, unsigned int initHeight = 400 );

			/** Class factory returning a smart pointer */
			static CDisplayWindowPtr Create(
				const std::string	&windowCaption = std::string() )
			{
				return CDisplayWindowPtr(new CDisplayWindow(windowCaption));
			}

			/** Destructor
			 */
			virtual ~CDisplayWindow();

			/** Show a given color or grayscale image on the window and print a set of points on it.
			 *  It adapts the size of the window to that of the image.
			 */
			void  showImageAndPoints( const CImage &img, const vector_float &x, const vector_float &y, const TColor &color = TColor::red );

			/** Show a given color or grayscale image on the window and print a set of points on it.
			 *  It adapts the size of the window to that of the image.
			 *  The class of FEATURELIST can be: mrpt::vision::CFeatureList
			 */
			template <class FEATURELIST>
			void  showImageAndPoints( const CImage &img, const FEATURELIST &list, const TColor &color = TColor::red, const bool &showIDs = false )
			{
				MRPT_START
				CImage imgColor(1,1,3);
				img.colorImage( imgColor );	// Create a colorimage

				for(typename FEATURELIST::const_iterator i = list.begin(); i != list.end(); ++i )
				{
					imgColor.cross( round((*i)->x), round((*i)->y), color, '+' );
					if( showIDs )
						imgColor.textOut( round((*i)->x), round((*i)->y), format("%u", static_cast<unsigned int>((*i)->ID)), 0xFF0000 );
					if( ! (*i)->isPointFeature() )
						imgColor.drawCircle( round((*i)->x), round((*i)->y), (*i)->scale, TColor::red );
				}

				showImage(imgColor);
				MRPT_END
			}

			/** Show a given color or grayscale image on the window and print a set of points on it and a set of lines splitting the image in tiles.
			 *  It adapts the size of the window to that of the image.
			 *  The class of FEATURELIST can be: mrpt::vision::CFeatureList
			 */
			template <class FEATURELIST>
			void  showTiledImageAndPoints( const CImage &img, const FEATURELIST &list, const TColor &color = TColor::red )
			{
				MRPT_START

				CImage imgColor(1,1,3);
				img.colorImage( imgColor ); // Create a colorimage

				// Print the 4 tile lines
				unsigned int w = imgColor.getWidth();
				unsigned int h = imgColor.getHeight();
				imgColor.line( 0, h/2, w-1, h/2, TColor::green );
				imgColor.line( w/4, 0, w/4, h, TColor::green );
				imgColor.line( w/2, 0, w/2, h, TColor::green );
				imgColor.line( 3*w/4, 0, 3*w/4, h, TColor::green );

				showImageAndPoints( imgColor, list, color );

				MRPT_END
			}

			/** Show a pair of given color or grayscale images (put together) on the window and print a set of matches on them.
			 *  It adapts the size of the window to that of the image.
			 *  MATCHEDLIST can be of the class: mrpt::vision::CMatchedFeatureList, or any STL container of pairs of anything having ".x" and ".y" (e.g. mrpt::math::TPoint2D)
			 */
			template <class MATCHEDLIST>
			void  showImagesAndMatchedPoints( const	CImage &img1, const	CImage &img2, const MATCHEDLIST &mList, const TColor &color = TColor::red )
			{
				MRPT_START

				CImage imgColor;

				//img1.colorImage( imgColor ); // Create a colorimage
				imgColor.joinImagesHorz( img1, img2 );

				unsigned int w = img1.getWidth();

				for( typename MATCHEDLIST::const_iterator i = mList.begin(); i != mList.end(); ++i )
				{
					imgColor.drawCircle( round( i->first->x ), round( i->first->y ), 4, color );
					imgColor.drawCircle( round( i->second->x + w ), round( i->second->y ), 4, color );
					imgColor.line( round( i->first->x ), round( i->first->y ), round( i->second->x + w ), round( i->second->y ), color );
				}
				showImage(imgColor);

				MRPT_END
			}


			/** Show a given color or grayscale image on the window.
			 *  It adapts the size of the window to that of the image.
			 */
			void  showImage( const CImage	&img );

			/** Plots a graph in MATLAB-like style.
			 */
			void  plot( const vector_float &x, const vector_float &y );

			/** Plots a graph in MATLAB-like style.
			 */
			void  plot( const vector_float &y );

			/** Resizes the window, stretching the image to fit into the display area.
			 */
			void  resize( unsigned int width, unsigned int height );

			/** Changes the position of the window on the screen.
			 */
			void  setPos( int x, int y );

			/** Enables or disables the visualization of cursor coordinates on the window caption (default = enabled).
			  */
			inline void  enableCursorCoordinatesVisualization(bool enable)
			{
				m_enableCursorCoordinates = enable;
			}

			/** Changes the window title text.
			  */
			void  setWindowTitle( const std::string &str );

		}; // End of class def.

	} // End of namespace

} // End of namespace

#endif
