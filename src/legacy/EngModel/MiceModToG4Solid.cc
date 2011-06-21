// MAUS WARNING: THIS IS LEGACY CODE.
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4Sphere.hh"
#include "G4Polycone.hh"
#include "G4Torus.hh"
#include "G4EllipticalCone.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4VSolid.hh"
#include "G4AffineTransform.hh"

#include "Interface/Squeal.hh"

#include "Config/MiceModule.hh"
#include "Config/ModuleTextFileIO.hh"

#include "EngModel/Polycone.hh"
#include "EngModel/MultipoleAperture.hh"
#include "EngModel/MiceModToG4Solid.hh"
#include <exception>

namespace MiceModToG4Solid
{


	//Build a G4Solid if you don't know the solid type
	G4VSolid* buildSolid ( MiceModule* mod )
	{
		std::string volume = mod->volType();
		for(unsigned int i=0; i<volume.size(); i++) volume[i] = tolower(volume[i]);
		if(volume == "none")      return NULL;
		if(volume == "wedge")     return buildWedge    (mod);
		if(volume == "box")       return buildBox      (mod);
		if(volume == "cylinder")  return buildCylinder (mod);
		if(volume == "tube")      return buildTube     (mod);
		if(volume == "sphere")    return buildSphere   (mod);
		if(volume == "polycone")  return buildPolycone (mod);
		if(volume == "torus")     return buildTorus    (mod);
		if(volume == "boolean")   return buildBoolean  (mod);
		if(volume == "multipole") return buildMultipole(mod);
		if(volume == "trapezoid") return buildTrapezoid(mod);

		if(volume == "ellipticalcone")  return buildEllipticalCone (mod);
		throw(Squeal(Squeal::recoverable, "Volume "+mod->volType()+" not recognised in module "+mod->fullName(), 
					"MiceModToG4Solid::buildSolid") );
	}


	//If you know the solid type, you may prefer to call these methods
	//One for each supported G4Solid
	G4VSolid * buildWedge    ( MiceModule* mod)
	{
		checkDim(mod, 3);
		return new G4Trd( mod->name()+"Wedge", mod->dimensions().x()/2., mod->dimensions().x()/2., 
				mod->dimensions().y(), 0, mod->dimensions().z()/2.);
	}

	G4VSolid * buildBox      ( MiceModule* mod)
	{
		checkDim(mod, 3);
		return new G4Box( mod->name() + "Box", mod->dimensions().x() / 2., mod->dimensions().y() / 2.,
				mod->dimensions().z() / 2. );
	}

	G4VSolid * buildCylinder ( MiceModule* mod )
	{
		checkDim(mod, 2);
		return new G4Tubs( mod->name() + "Tubs", 0., mod->dimensions().x(), mod->dimensions().y() / 2., 0. * deg, 360. * deg );
	}

	G4VSolid * buildTube     ( MiceModule* mod )
	{
		checkDim(mod, 3);
		if(mod->dimensions().x() > mod->dimensions().y()) 
			throw(Squeal(Squeal::recoverable, "Inner radius > outer radius of tube made from module "+mod->fullName(), "MiceModToG4Solid::buildTube") );
		return new G4Tubs( mod->name() + "Tubs", mod->dimensions().x(), mod->dimensions().y(), mod->dimensions().z() / 2., 0. * deg, 360. * deg );
	}

	G4VSolid * buildEllipticalCone   ( MiceModule* mod )
	{
		double zcut = 2.*mod->dimensions().z();
		if( mod->propertyExistsThis( "ZCut", "double" )) {
			if( mod->propertyDouble( "ZCut" )>0.) 
				zcut = mod->propertyDoubleThis("ZCut");
			else 
				throw(Squeal(Squeal::recoverable, "Elliptical cone ZCut should be > 0.", "MiceModule::buildEllipticalCone"));
		}
		G4EllipticalCone * out = new G4EllipticalCone(mod->name() + "Cone", mod->dimensions().x()/mod->dimensions().z(), 
				mod->dimensions().y()/mod->dimensions().z(), mod->dimensions().z(), zcut);
		return out;
	}


	G4VSolid * buildSphere   ( MiceModule* mod )
	{
		Hep3Vector phi, theta;

		if( mod->propertyExistsThis( "Phi", "Hep3Vector" )) 
			phi = mod->propertyHep3VectorThis("Phi");
		else	phi = Hep3Vector(0.0*deg, 360.0*deg, -1.0);
		if(phi.x() < 0.*deg || phi.x() > 360.*deg || phi.y() < 0.*deg || phi.y() > 360.*deg)
			throw(Squeal(Squeal::recoverable, "Phi out of range in sphere module "+mod->fullName(), "MiceModToG4Solid::buildSphere") );

		if( mod->propertyExistsThis( "Theta", "Hep3Vector" ))
			theta = mod->propertyHep3VectorThis("Theta");
		else	theta = Hep3Vector(0.0*deg, 180.0*deg, -1.0);
		if(theta.x() < 0.*deg || theta.x() > 180.*deg || theta.y() < 0.*deg || theta.y() > 180.*deg)
			throw(Squeal(Squeal::recoverable, "Theta out of range in sphere module "+mod->fullName(), "MiceModToG4Solid::buildSphere") );

		if( mod->dimensions().x() >= mod->dimensions().y() )
			throw(Squeal(Squeal::recoverable, "Inner radius > outer radius of sphere made from module "+mod->fullName(), "MiceModToG4Solid::buildSphere") );
		checkDim(mod, 2);
		return new G4Sphere(mod->name() + "Sphere", mod->dimensions().x(), mod->dimensions().y(), phi.x(), phi.y(), theta.x(), theta.y());
	}

	G4VSolid * buildPolycone ( MiceModule* mod )
	{
		Polycone newPolycone(mod);
		return newPolycone.GetPolycone();
	}

	G4VSolid * buildTorus    ( MiceModule* mod )
	{
		double pRmin      = mod->propertyDoubleThis("TorusInnerRadius");
		double pRmax      = mod->propertyDoubleThis("TorusOuterRadius");
		double rCurv      = mod->propertyDoubleThis("TorusRadiusOfCurvature");
		double angleStart = mod->propertyDoubleThis("TorusAngleStart");
		double openAngle  = mod->propertyDoubleThis("TorusOpeningAngle");
		if(pRmin < 0.)    throw(Squeal(Squeal::recoverable, "Negative torus inner radius not allowed in module "+mod->fullName(), "MiceModToG4Solid::buildTorus"));
		if(pRmax < pRmin) throw(Squeal(Squeal::recoverable, "Torus outer radius should be > torus inner radius in module "+mod->fullName(), "MiceModToG4Solid::buildTorus"));
		if(pRmax > rCurv) throw(Squeal(Squeal::recoverable, "Torus outer radius should be < torus radius of curvature in module "+mod->fullName(), "MiceModToG4Solid::buildTorus"));
		if(angleStart < 0. || angleStart > 360.*deg) 
			throw(Squeal(Squeal::recoverable, "Torus start angle out of range in module "+mod->fullName(), "MiceModToG4Solid::buildTorus"));
		if(openAngle <= 0. || openAngle > 360.*deg) 
			throw(Squeal(Squeal::recoverable, "Torus start angle out of range in module "+mod->fullName(), "MiceModToG4Solid::buildTorus"));
		return new G4Torus( mod->name() + "TorusSegment", pRmin, pRmax, rCurv, angleStart, openAngle );
	}

	G4VSolid * buildTrapezoid    ( MiceModule* mod ) //should have x1, x2, y1, y2, z
	{ 	
		double widthX1, widthX2, heightY1, heightY2, lengthZ;
		try{
			double widthX1    = mod->propertyDouble( "TrapezoidWidthX1" );
			double widthX2    = mod->propertyDouble( "TrapezoidWidthX2" );
			double heightY1   = mod->propertyDouble( "TrapezoidHeightY1" );
			double heightY2   = mod->propertyDouble( "TrapezoidHeightY2" );
			double lengthZ    = mod->propertyDouble( "TrapezoidLengthZ" );
			if( widthX1 < 0. || widthX2 < 0. || heightY1 < 0. || heightY2 < 0. || lengthZ < 0. )  throw( Squeal( Squeal::recoverable, "Negative parameter for dimension in  "+mod->fullName(), "MiceModToG4Solid::buildTrapezoid"));
			return new G4Trd( mod->name() + "Trapezoid", widthX1, widthX2, heightY1, heightY2, lengthZ );
		}
		catch(Squeal squee){
		      std::string error = squee.GetMessage();
		      throw(Squeal(Squeal::recoverable, "Error building a trapezoid: " + error + "\'", "MiceModToG4Solid::buildTrapezoid"));
		}
	}



	G4VSolid * buildMultipole( MiceModule* mod )
	{
		MultipoleAperture multipole(mod);
		G4VSolid*    ap = multipole.getMultipoleAperture();
		return       ap;
	}

	G4VSolid * buildBoolean  ( MiceModule* mod )
	{
		MiceModule* base      = getModule(mod, mod->propertyStringThis("BaseModule"));
		G4VSolid*   baseSolid = buildSolid(base);
		std::string boolProp  = "BooleanModule1";
		int         index     = 1;
		while(mod->propertyExistsThis(boolProp, "string"))
		{
			MiceModule*         next   = getModule(mod, mod->propertyStringThis(boolProp));
			Squeak::mout(Squeak::debug) << "Adding module " << next->fullName() << " for boolean " << mod->fullName() << std::endl;
			std::string         type   = mod->propertyStringThis    (boolProp+"Type");
			G4ThreeVector       pos    = mod->propertyHep3VectorThis(boolProp+"Pos");
			CLHEP::Hep3Vector   rTemp  = mod->propertyHep3VectorThis(boolProp+"Rot");
			CLHEP::HepRotation* rot    = new CLHEP::HepRotation(HepRotationX( rTemp.x() )* 
					HepRotationY( rTemp.y() )*HepRotationZ( rTemp.z() ));
			G4VSolid*           solid  = buildSolid(next);
			G4DisplacedSolid*   moved  = new G4DisplacedSolid("moved", solid, G4AffineTransform(rot, pos));
			/**/ if(type == "Union")        baseSolid = new G4UnionSolid       ("base", baseSolid, moved);
			else if(type == "Intersection") baseSolid = new G4IntersectionSolid("base", baseSolid, moved);
			else if(type == "Subtraction")  baseSolid = new G4SubtractionSolid ("base", baseSolid, moved);
			else throw(Squeal(Squeal::recoverable, "Did not recognise boolean type "+type, 
						"MiceModToG4Solid::buildBoolean"));
			index++;
			std::stringstream iStream;
			iStream << "BooleanModule" << index;
			boolProp = iStream.str();
			mod->removeDaughter(next);
			delete next;
			delete rot;
			Squeak::mout(Squeak::debug) << "Searching for " << iStream.str() << std::endl;
		}
		mod->removeDaughter(base);

		delete base;
		return baseSolid;
	}

	MiceModule* getModule      (MiceModule* mod, std::string newModName)
	{
		std::stringstream modStream1, modStream2;
		std::string       file = std::string(getenv( "MICEFILES" )) + "/Models/Modules/" + newModName;
		std::ifstream     fin(file.c_str());
		if(!fin) 
		{
			fin.close();
			fin.open(newModName.c_str());
			if(!fin) 
			{
				fin.close();
				throw(Squeal(Squeal::recoverable, "Failed to open either file "+file+" or file "+newModName+" for boolean "+mod->fullName(),
							"MiceModToG4Solid::getModule"));
			}
		}
		ModuleTextFileIO::stripFile(modStream1, fin);
		ModuleTextFileIO::substitute(modStream2, modStream1);
		ModuleTextFileIO daughter  (mod, newModName, modStream2);
		return daughter.getModule  ();
	}

	void checkDim(MiceModule* mod, size_t length) {
		std::string axes[] = {"x", "y", "z"};
		for(size_t i=0; i<length && i<3; i++)
			if(mod->dimensions()[i] <= 0.) {
				throw(Squeal(Squeal::recoverable, "Dimension "+axes[i]+" in module "+mod->fullName()+" out of range", "MiceModToG4Solid::checkDim"));
			}
	}


}






