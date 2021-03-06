/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2017 OpenFOAM Foundation
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

\*---------------------------------------------------------------------------*/

#include "mojThermo.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class Thermo, template<class> class Type>
inline Foam::species::mojThermo<Thermo, Type>::mojThermo
(
    const Thermo& sp
)
:
    Thermo(sp)
{}


template<class Thermo, template<class> class Type>
inline Foam::scalar Foam::species::mojThermo<Thermo, Type>::T
(
    scalar f,
    scalar p,
    scalar T0,
    scalar (mojThermo<Thermo, Type>::*F)(const scalar, const scalar) const,
    scalar (mojThermo<Thermo, Type>::*dFdT)(const scalar, const scalar)
        const,
    scalar (mojThermo<Thermo, Type>::*limit)(const scalar) const
) const
{
    if (T0 < 0)
    {
        FatalErrorInFunction
            << "Negative initial temperature T0: " << T0
            << abort(FatalError);
    }

    scalar Test = T0;
    scalar Tnew = T0;
    scalar dlFtest; // for bisection - Kristjan 
    scalar dlFnew; // for bisection - Kristjan
    scalar Ttol = T0*tol_;
    int    iter = 0;

    dlFnew = (this->*F)(p, Tnew) - f;
    do
    {
        Test = Tnew;
        dlFtest = dlFnew;
        Tnew =
            (this->*limit)
            (Test - dlFtest/(this->*dFdT)(p, Test));

        if (iter > maxIter_/2)
        {
            Info << "f= " << f << " p= " << p << " *F=" << (this->*F)(p, Test) << " Tnew=" << Tnew << " *dFdT=" << (this->*dFdT)(p, Test) << endl;
        }

        dlFnew = (this->*F)(p, Tnew) - f;

        if (dlFtest*dlFnew  < -SMALL)//quasi bisection - Kristjan
        {
            Tnew = (Test*dlFnew - Tnew*dlFtest)/(dlFnew-dlFtest);
            iter++;

            if (mag(Tnew - Test) > Ttol)
            {
                dlFnew = (this->*F)(p, Tnew) - f;
            }

            if (iter > maxIter_/2)
            {
                Info << "f= " << f << " p= " << p << " *F=" << (this->*F)(p, Test) << " Tnew=" << Tnew << " *dFdT=" << (this->*dFdT)(p, Test) << " b" << endl;
            }

            if (iter > maxIter_)
            {
                FatalErrorInFunction
                    << "Maximum number of iterations exceeded"
                    << abort(FatalError);
            }

        }

        if (iter++ > maxIter_)
        {
            FatalErrorInFunction
                << "Maximum number of iterations exceeded: " << maxIter_
                << abort(FatalError);
        }

    } while (mag(Tnew - Test) > Ttol);

    return Tnew;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Thermo, template<class> class Type>
inline Foam::species::mojThermo<Thermo, Type>::mojThermo
(
    const word& name,
    const mojThermo& st
)
:
    Thermo(name, st)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Thermo, template<class> class Type>
inline Foam::word
Foam::species::mojThermo<Thermo, Type>::heName()
{
    return Type<mojThermo<Thermo, Type>>::name();
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::Cv(const scalar p, const scalar T) const
{
    return this->Cp(p, T) - this->CpMCv(p, T);
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::Cpv(const scalar p, const scalar T) const
{
    return Type<mojThermo<Thermo, Type>>::Cpv(*this, p, T);
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::gamma(const scalar p, const scalar T) const
{
    const scalar Cp = this->Cp(p, T);
    return Cp/(Cp - this->CpMCv(p, T));
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::CpByCpv
(
    const scalar p,
    const scalar T
) const
{
    return Type<mojThermo<Thermo, Type>>::CpByCpv(*this, p, T);
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::HE(const scalar p, const scalar T) const
{
    return Type<mojThermo<Thermo, Type>>::HE(*this, p, T);
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::Es(const scalar p, const scalar T) const
{
    return this->Hs(p, T) - p/this->rho(p, T);
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::Ea(const scalar p, const scalar T) const
{
    return this->Ha(p, T) - p/this->rho(p, T);
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::G(const scalar p, const scalar T) const
{
    return this->Ha(p, T) - T*this->S(p, T);
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::A(const scalar p, const scalar T) const
{
    return this->Ea(p, T) - T*this->S(p, T);
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::cp(const scalar p, const scalar T) const
{
    return this->Cp(p, T)*this->W();
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::ha(const scalar p, const scalar T) const
{
    return this->Ha(p, T)*this->W();
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::hs(const scalar p, const scalar T) const
{
    return this->Hs(p, T)*this->W();
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::hc() const
{
    return this->Hc()*this->W();
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::s(const scalar p, const scalar T) const
{
    return this->S(p, T)*this->W();
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::he(const scalar p, const scalar T) const
{
    return this->HE(p, T)*this->W();
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::cv(const scalar p, const scalar T) const
{
    return this->Cv(p, T)*this->W();
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::es(const scalar p, const scalar T) const
{
    return this->Es(p, T)*this->W();
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::ea(const scalar p, const scalar T) const
{
    return this->Ea(p, T)*this->W();
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::g(const scalar p, const scalar T) const
{
    return this->G(p, T)*this->W();
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::a(const scalar p, const scalar T) const
{
    return this->A(p, T)*this->W();
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::K(const scalar p, const scalar T) const
{
    scalar arg = -this->Y()*this->G(Pstd, T)/(RR*T);

    if (arg < 600)
    {
        return exp(arg);
    }
    else
    {
        return VGREAT;
    }
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::Kp(const scalar p, const scalar T) const
{
    return K(p, T);
}


template<class Thermo, template<class> class Type>
inline Foam::scalar
Foam::species::mojThermo<Thermo, Type>::Kc(const scalar p, const scalar T) const
{
    const scalar nm = this->Y()/this->W();

    if (equal(nm, SMALL))
    {
        return Kp(p, T);
    }
    else
    {
        return Kp(p, T)*pow(Pstd/(RR*T), nm);
    }
}


template<class Thermo, template<class> class Type>
inline Foam::scalar Foam::species::mojThermo<Thermo, Type>::Kx
(
    const scalar p,
    const scalar T
) const
{
    const scalar nm = this->Y()/this->W();

    if (equal(nm, SMALL))
    {
        return Kp(p, T);
    }
    else
    {
        return Kp(p, T)*pow(Pstd/p, nm);
    }
}


template<class Thermo, template<class> class Type>
inline Foam::scalar Foam::species::mojThermo<Thermo, Type>::Kn
(
    const scalar p,
    const scalar T,
    const scalar n
) const
{
    const scalar nm = this->Y()/this->W();

    if (equal(nm, SMALL))
    {
        return Kp(p, T);
    }
    else
    {
        return Kp(p, T)*pow(n*Pstd/p, nm);
    }
}


template<class Thermo, template<class> class Type>
inline Foam::scalar Foam::species::mojThermo<Thermo, Type>::THE
(
    const scalar he,
    const scalar p,
    const scalar T0
) const
{
    return Type<mojThermo<Thermo, Type>>::THE(*this, he, p, T0);
}


template<class Thermo, template<class> class Type>
inline Foam::scalar Foam::species::mojThermo<Thermo, Type>::THs
(
    const scalar hs,
    const scalar p,
    const scalar T0
) const
{
    return T
    (
        hs,
        p,
        T0,
        &mojThermo<Thermo, Type>::Hs,
        &mojThermo<Thermo, Type>::Cp,
        &mojThermo<Thermo, Type>::limit
    );
}


template<class Thermo, template<class> class Type>
inline Foam::scalar Foam::species::mojThermo<Thermo, Type>::THa
(
    const scalar ha,
    const scalar p,
    const scalar T0
) const
{
    return T
    (
        ha,
        p,
        T0,
        &mojThermo<Thermo, Type>::Ha,
        &mojThermo<Thermo, Type>::Cp,
        &mojThermo<Thermo, Type>::limit
    );
}


template<class Thermo, template<class> class Type>
inline Foam::scalar Foam::species::mojThermo<Thermo, Type>::TEs
(
    const scalar es,
    const scalar p,
    const scalar T0
) const
{
    return T
    (
        es,
        p,
        T0,
        &mojThermo<Thermo, Type>::Es,
        &mojThermo<Thermo, Type>::Cv,
        &mojThermo<Thermo, Type>::limit
    );
}


template<class Thermo, template<class> class Type>
inline Foam::scalar Foam::species::mojThermo<Thermo, Type>::TEa
(
    const scalar ea,
    const scalar p,
    const scalar T0
) const
{
    return T
    (
        ea,
        p,
        T0,
        &mojThermo<Thermo, Type>::Ea,
        &mojThermo<Thermo, Type>::Cv,
        &mojThermo<Thermo, Type>::limit
    );
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template<class Thermo, template<class> class Type>
inline void Foam::species::mojThermo<Thermo, Type>::operator+=
(
    const mojThermo<Thermo, Type>& st
)
{
    Thermo::operator+=(st);
}


template<class Thermo, template<class> class Type>
inline void Foam::species::mojThermo<Thermo, Type>::operator*=(const scalar s)
{
    Thermo::operator*=(s);
}


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

template<class Thermo, template<class> class Type>
inline Foam::species::mojThermo<Thermo, Type> Foam::species::operator+
(
    const mojThermo<Thermo, Type>& st1,
    const mojThermo<Thermo, Type>& st2
)
{
    return mojThermo<Thermo, Type>
    (
        static_cast<const Thermo&>(st1) + static_cast<const Thermo&>(st2)
    );
}


template<class Thermo, template<class> class Type>
inline Foam::species::mojThermo<Thermo, Type> Foam::species::operator*
(
    const scalar s,
    const mojThermo<Thermo, Type>& st
)
{
    return mojThermo<Thermo, Type>
    (
        s*static_cast<const Thermo&>(st)
    );
}


template<class Thermo, template<class> class Type>
inline Foam::species::mojThermo<Thermo, Type> Foam::species::operator==
(
    const mojThermo<Thermo, Type>& st1,
    const mojThermo<Thermo, Type>& st2
)
{
    return mojThermo<Thermo, Type>
    (
        static_cast<const Thermo&>(st1) == static_cast<const Thermo&>(st2)
    );
}


// ************************************************************************* //
