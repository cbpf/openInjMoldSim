/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2012 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

 scalar n_, Tau_, D1_, D2_, D3_, A1_, A2_;

\*---------------------------------------------------------------------------*/

#include "crossWLFTransport.H"
#include "IOstreams.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Thermo>
Foam::crossWLFTransport<Thermo>::crossWLFTransport(Istream& is)
:
    Thermo(is),
    n_(readScalar(is)),
    Tau_(readScalar(is)),
    D1_(readScalar(is)),
    D2_(readScalar(is)),
    D3_(readScalar(is)),
    A1_(readScalar(is)),
    A2_(readScalar(is)),
    kappa_(readScalar(is)),
    etaMin_(readScalar(is)),
    etaMax_(readScalar(is)),
    TnoFlow_(readScalar(is)),
    deltaTempInterp_(readScalar(is))
{
    is.check("crossWLFTransport<Thermo>::crossWLFTransport(Istream&)");
}


template<class Thermo>
Foam::crossWLFTransport<Thermo>::crossWLFTransport(const dictionary& dict)
:
    Thermo(dict),
    n_(readScalar(dict.subDict("transport").lookup("n"))),
    Tau_(readScalar(dict.subDict("transport").lookup("Tau"))),
    D1_(readScalar(dict.subDict("transport").lookup("D1"))),
    D2_(readScalar(dict.subDict("transport").lookup("D2"))),
    D3_(readScalar(dict.subDict("transport").lookup("D3"))),
    A1_(readScalar(dict.subDict("transport").lookup("A1"))),
    A2_(readScalar(dict.subDict("transport").lookup("A2"))),
    kappa_(readScalar(dict.subDict("transport").lookup("kappa"))),
    etaMin_(readScalar(dict.subDict("transport").lookup("etaMin"))),
    etaMax_(readScalar(dict.subDict("transport").lookup("etaMax"))),
    TnoFlow_(readScalar(dict.subDict("transport").lookup("TnoFlow"))),
    deltaTempInterp_(dict.subDict("transport").lookupOrDefault<scalar>("deltaTempInterp", 5.0))
{
    Info << "CrossWLF:" << endl;
    Info << "n               : " << n_              << endl;
    Info << "Tau             : " << Tau_            << endl;
    Info << "D1              : " << D1_             << endl;
    Info << "D2              : " << D2_             << endl;
    Info << "D3              : " << D3_             << endl;
    Info << "A1              : " << A1_             << endl;
    Info << "A2              : " << A2_             << endl;
    Info << "kappa           : " << kappa_         << endl;
    Info << "etaMin          : " << etaMin_         << endl;
    Info << "etaMax          : " << etaMax_         << endl;
    Info << "TnoFlow         : " << TnoFlow_        << endl;
    Info << "deltaTempInterp : " << deltaTempInterp_<< endl << endl;
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Thermo>
void Foam::crossWLFTransport<Thermo>::write(Ostream& os) const
{
    os  << this->specie::name() << endl;
    os  << token::BEGIN_BLOCK  << incrIndent << nl;

    Thermo::write(os);

    dictionary dict("transport");
    dict.add("n", n_);
    dict.add("Tau", Tau_);
    dict.add("D1", D1_);
    dict.add("D2", D2_);
    dict.add("D3", D3_);
    dict.add("A1", A1_);
    dict.add("A2", A2_);
    dict.add("kappa", kappa_);
    dict.add("etaMin", etaMin_);
    dict.add("etaMax", etaMax_);
    dict.add("TnoFlow", TnoFlow_);
    dict.add("deltaTempInterp", deltaTempInterp_);
    os  << indent << dict.dictName() << dict;

    os  << decrIndent << token::END_BLOCK << nl;
}

// * * * * * * * * * * * * * * * IOstream Operators  * * * * * * * * * * * * //

template<class Thermo>
Foam::Ostream& Foam::operator<<
(
    Ostream& os,
    const crossWLFTransport<Thermo>& st
)
{
    os << static_cast<const Thermo&>(st)
    << tab << st.n_
    << tab << st.Tau_
    << tab << st.D1_
    << tab << st.D2_
    << tab << st.D3_
    << tab << st.A1_
    << tab << st.A2_
    << tab << st.etaMin_
    << tab << st.etaMax_
    << tab << st.TnoFlow_
    << tab << st.kappa_
    << tab << st.deltaTempInterp_ << endl;

    os.check
    (
        "Ostream& operator<<(Ostream&, const crossWLFTransport<Thermo>&)"
    );

    return os;
}


// ************************************************************************* //
