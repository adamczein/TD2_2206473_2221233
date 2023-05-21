#pragma once
#include "ListeJeux.hpp"
#include <string>
#include "Jeu.hpp"
#include "gsl/span"
#include <iostream>

using namespace std;
using namespace gsl;

class Developpeur
{
public:
    // Constructeurs
    Developpeur();
    Developpeur(const string& nom);

    // Accesseur
    const string& obtenirNom() const;

    // Méthodes
    int compterNombreJeux(const ListeJeux& listeJeux) const;
    void mettreDeveloppeurAJour(ListeJeux& listeTousLesJeux);
    void afficherJeuxDeveloppes();

    // Destructeur
    ~Developpeur();

private:
    // Membres de données
    string nom_;
    ListeJeux jeuxDeveloppes_;
};

Developpeur::Developpeur()
    : nom_("Pas de nom"), jeuxDeveloppes_()
{
}

Developpeur::Developpeur(const string& nom)
    : nom_(nom), jeuxDeveloppes_()
{
}

const string& Developpeur::obtenirNom() const
{
    return nom_;
}

int Developpeur::compterNombreJeux(const ListeJeux& listeJeux) const
{
    int nbJeux = 0;
    for (Jeu* ptrJeu : span(listeJeux.elements, listeJeux.nElements))
    {
        if (ptrJeu->developpeur == obtenirNom())
            nbJeux++;
    }
    return nbJeux;
}

void Developpeur::mettreDeveloppeurAJour(ListeJeux& listeTousLesJeux)
{
    int tailleListeJeux = compterNombreJeux(listeTousLesJeux);

    // Supprimer l'ancien tableau de jeux développés
    delete[] jeuxDeveloppes_.elements;

    // Créer un nouveau tableau de jeux développés avec la taille appropriée
    jeuxDeveloppes_.elements = new Jeu * [tailleListeJeux];
    jeuxDeveloppes_.capacite = tailleListeJeux;
    jeuxDeveloppes_.nElements = 0;

    // Parcourir tous les jeux de la liste complète des jeux
    for (Jeu* ptrJeu : span(listeTousLesJeux.elements, listeTousLesJeux.nElements))
    {
        if (ptrJeu->developpeur == obtenirNom())
        {
            // Ajouter le jeu développé à la nouvelle liste des jeux développés
            jeuxDeveloppes_.elements[jeuxDeveloppes_.nElements] = ptrJeu;
            jeuxDeveloppes_.nElements++;
        }
    }
}

void Developpeur::afficherJeuxDeveloppes()
{
    cout << "Les jeux développés par " << nom_ << " sont : " << endl;

    // Parcourir et afficher tous les jeux de la liste des jeux développés
    for (Jeu* ptrJeu : span(jeuxDeveloppes_.elements, jeuxDeveloppes_.nElements))
    {
        cout << "\t" << ptrJeu->titre << endl;
    }
}

Developpeur::~Developpeur()
{
    // Libérer la mémoire du tableau des jeux développés
    delete[] jeuxDeveloppes_.elements;
}