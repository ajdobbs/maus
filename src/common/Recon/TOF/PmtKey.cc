// PmtKey.cc
// Mark Rayner, January 2009	
// A key to identify individual PMTs in maps of calibration constants etc

#include "PmtKey.hh"

PmtKey::PmtKey( int station, int plane, int slab, int pmt ) {
	_station = station;
	_plane = plane;
	_slab = slab;
	_pmt = pmt;
}

bool PmtKey::operator== ( PmtKey const key ) {
	if ( _station == key.station() && _plane == key.plane() && _slab == key.slab() && _pmt == key.pmt() ) {
		return true;
	}
	else return false;
}

ostream& operator<< ( ostream& stream, PmtKey key ) {
	stream << "PmtKey: station " << key._station;
	stream << ", plane " << key._plane;
	stream << ", slab " << key._slab;
	stream << ", pmt " << key._pmt;
	return stream;
}