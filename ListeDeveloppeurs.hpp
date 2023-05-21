#pragma once
#include <cstddef>
#include "Developpeur.hpp"
#include "gsl/span"

using namespace gsl;
using namespace std;

class ListeDeveloppeurs
{
public:
    ListeDeveloppeurs();
    void afficher() const;
    void ajouterDeveloppeur(Developpeur* ptrDeveloppeur);
    void retirerDeveloppeur(Developpeur* ptrDeveloppeur);
    ~ListeDeveloppeurs();

private:
    size_t nElements_;    // Nombre d'éléments dans la liste
    size_t capacite_;     // Capacité de la liste (taille maximale)
    Developpeur** elements_;    // Tableau de pointeurs vers des objets Developpeur
};

ListeDeveloppeurs::ListeDeveloppeurs()
{
    elements_ = nullptr;    // Initialisation du tableau à nullptr
    nElements_ = 0;         // Initialisation du nombre d'éléments à 0
    capacite_ = 0;          // Initialisation de la capacité à 0
}

void ListeDeveloppeurs::afficher() const
{
    cout << "--------------------------------------------------" << endl;
    cout << "La liste des développeurs contient " << nElements_ << " développeur(s)" << endl;

    // Parcourir le tableau d'éléments et afficher les jeux développés par chaque développeur
    for (Developpeur* developpeur : span(elements_, nElements_))
        developpeur->afficherJeuxDeveloppes();
}

void ListeDeveloppeurs::ajouterDeveloppeur(Developpeur* ptrDeveloppeur)
{
    bool estListeVide = capacite_ == 0;
    bool estListePleine = capacite_ - 1 <= nElements_;

    if (estListePleine || estListeVide)
    {
        // Calculer la nouvelle capacité en doublant la capacité actuelle si la liste est vide, sinon
        // en la multipliant par 2
        size_t nouvelleCapacite = estListeVide ? 1 : capacite_ * 2;

        // Allouer un nouveau tableau de pointeurs de la taille de la nouvelle capacité
        Developpeur** nouvelleListeDeveloppeurs = new Developpeur * [nouvelleCapacite];

        // Copier les pointeurs des développeurs du tableau actuel vers le nouveau tableau
        for (size_t i = 0; i < nElements_; i++)
            nouvelleListeDeveloppeurs[i] = elements_[i];

        // Libérer la mémoire du tableau actuel
        delete[] elements_;

        // Mettre à jour le tableau et la capacité avec le nouveau tableau et la nouvelle capacité
        elements_ = nouvelleListeDeveloppeurs;
        capacite_ = nouvelleCapacite;
    }

    // Ajouter le pointeur du nouveau développeur à la fin du tableau
    elements_[nElements_] = ptrDeveloppeur;
    nElements_++;
}

void ListeDeveloppeurs::retirerDeveloppeur(Developpeur* ptrDeveloppeur)
{
    int indexDeveloppeur = -1;

    // Rechercher l'index du développeur à retirer dans le tableau d'éléments
    for (int i = 0; i < nElements_; i++)
    {
        if (elements_[i] == ptrDeveloppeur)
            indexDeveloppeur = i;
    }

    bool estDeveloppeurPresent = indexDeveloppeur != -1;
    bool estListeVide = nElements_ <= 0;

    if (estDeveloppeurPresent && !estListeVide)
    {
        // Échanger le développeur à retirer avec le dernier développeur du tableau
        Developpeur* ptrDernierDeveloppeur = elements_[nElements_ - 1];
        elements_[indexDeveloppeur] = ptrDernierDeveloppeur;
        elements_[nElements_ - 1] = nullptr;
        nElements_--;
    }
}

ListeDeveloppeurs::~ListeDeveloppeurs()
{
    // Libérer la mémoire du tableau d'éléments
    delete[] elements_;
}