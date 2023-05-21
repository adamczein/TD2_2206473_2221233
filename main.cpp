#include "Jeu.hpp"
#include <iostream>
#include <fstream>
#include <cstdint>
#include "cppitertools/range.hpp"
#include "gsl/span"
#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp"
#include "debogage_memoire.hpp"
#include "Developpeur.hpp"
#include "ListeDeveloppeurs.hpp"

using namespace std;
using namespace iter;
using namespace gsl;

#pragma region "Fonctions de base pour vous aider"
typedef uint8_t UInt8;
typedef uint16_t UInt16;
UInt8 lireUint8(istream& fichier)
{
    UInt8 valeur = 0;
    fichier.read((char*)&valeur, sizeof(valeur));
    return valeur;
}
UInt16 lireUint16(istream& fichier)
{
    UInt16 valeur = 0;
    fichier.read((char*)&valeur, sizeof(valeur));
    return valeur;
}
string lireString(istream& fichier)
{
    string texte;
    texte.resize(lireUint16(fichier));
    fichier.read((char*)&texte[0], streamsize(sizeof(texte[0])) * texte.length());
    return texte;
}
gsl::span<Jeu*> spanListeJeux(const ListeJeux& liste)
{
    return gsl::span(liste.elements, liste.nElements);
}
gsl::span<Designer*> spanListeDesigners(const ListeDesigners& liste)
{
    return gsl::span(liste.elements, liste.nElements);
}
#pragma endregion

//chnage
Designer* chercherDesigner(const string& nomDesigner, const ListeJeux& listeJeux)
{
    Designer* ptrDesigner = nullptr;
    for (Jeu* ptrJeu : spanListeJeux(listeJeux))
    {
        for (Designer* tempPtrDesigner : spanListeDesigners(ptrJeu->designers))
        {
            if (tempPtrDesigner->nom == nomDesigner)
            {
                ptrDesigner = tempPtrDesigner;
            }
        }
    };
    return ptrDesigner;
}

Designer* lireDesigner(istream& fichier, const ListeJeux& listeJeux)
{
    Designer designer = {};
    designer.nom = lireString(fichier);
    designer.anneeNaissance = int(lireUint16(fichier));
    designer.pays = lireString(fichier);

    Designer* ptrDesigner = chercherDesigner(designer.nom, listeJeux);

    if (ptrDesigner == nullptr)
        ptrDesigner = new Designer(designer);

    cout << "L'allocation du designer " << ptrDesigner->nom << " est réussie." << endl;
    return ptrDesigner;
}

void changerTailleListeJeux(size_t nouvelleCapacite, ListeJeux& listeJeux)
{
    Jeu** nouvelleListeJeux = new Jeu * [nouvelleCapacite];

    for (size_t i = 0; i < listeJeux.nElements; i++)
        nouvelleListeJeux[i] = listeJeux.elements[i];

    delete[] listeJeux.elements;

    listeJeux.elements = nouvelleListeJeux;
    listeJeux.capacite = nouvelleCapacite;
}

void ajouterJeu(Jeu* jeu, ListeJeux& listeJeux)
{
    bool estListePleine = listeJeux.capacite - 1 <= listeJeux.nElements;
    bool estListeNouvelle = listeJeux.capacite == 0;

    if (estListePleine || estListeNouvelle)
    {
        size_t nouvelleCapacite = estListeNouvelle ? listeJeux.capacite = 1 : listeJeux.capacite * 2;
        changerTailleListeJeux(nouvelleCapacite, listeJeux);
    }

    listeJeux.elements[listeJeux.nElements] = jeu;
    listeJeux.nElements++;
}

void enleveJeuListe(Jeu* ptrJeu, ListeJeux& listeJeux)
{
    int indexJeu = -1;
    size_t nElements = listeJeux.nElements;

    for (int i = 0; i < nElements; i++)
    {
        if (listeJeux.elements[i] == ptrJeu)
            indexJeu = i;
    }

    bool estJeuPresent = indexJeu != -1;
    bool estListeVide = nElements <= 0;

    if (estJeuPresent && !estListeVide)
    {
        Jeu* ptrDernierJeu = listeJeux.elements[nElements - 1];
        listeJeux.elements[indexJeu] = ptrDernierJeu;
        listeJeux.elements[nElements - 1] = nullptr;
        listeJeux.nElements--;
    }
}

Jeu* lireJeu(istream& fichier, const ListeJeux& listeJeux)
{
    Jeu jeu = {};
    jeu.titre = lireString(fichier);
    jeu.anneeSortie = int(lireUint16(fichier));
    jeu.developpeur = lireString(fichier);
    jeu.designers.nElements = lireUint8(fichier);

    Jeu* ptrJeu = new Jeu(jeu);
    ptrJeu->designers.elements = new Designer * [jeu.designers.nElements];

    for ([[maybe_unused]] size_t i : iter::range(jeu.designers.nElements))
    {
        Designer* ptrDesigner = lireDesigner(fichier, listeJeux);

        ptrJeu->designers.elements[i] = ptrDesigner;
        ajouterJeu(ptrJeu, ptrDesigner->listeJeuxParticipes);
    }
    cout << "L'allocation du jeu " << ptrJeu->titre << " est réussie." << endl;
    return ptrJeu;
}

ListeJeux creerListeJeux(const string& nomFichier)
{
    ifstream fichier(nomFichier, ios::binary);
    fichier.exceptions(ios::failbit);
    size_t nElements = lireUint16(fichier);
    ListeJeux listeJeux = {};

    for ([[maybe_unused]] size_t n : iter::range(nElements))
    {
        Jeu* ptrJeu = lireJeu(fichier, listeJeux);
        ajouterJeu(ptrJeu, listeJeux);
    }

    return listeJeux;
}

void detruireDesigner(Designer* ptrDesigner)
{
    cout << "Le pointeur du designer " << ptrDesigner->nom << "a été detruit" << endl;
    delete[] ptrDesigner->listeJeuxParticipes.elements;
}

bool designerParticipeJeu(Jeu* ptrJeu, Designer* ptrDesigner)
{
    bool estParticipantJeu = false;
    for (Designer* tempptrDesigner : spanListeDesigners(ptrJeu->designers))
        if (tempptrDesigner == ptrDesigner)
            estParticipantJeu = true;


    return estParticipantJeu;
}

void detruirejeu(Jeu* ptrJeu, ListeJeux& listeJeu)
{
    for (Designer* ptrDesigner : spanListeDesigners(ptrJeu->designers))
    {
        bool estDernierJeu = ptrDesigner->listeJeuxParticipes.nElements <= 1;
        if (estDernierJeu)
        {
            detruireDesigner(ptrDesigner);
            delete ptrDesigner;
        }
        else
        {
            enleveJeuListe(ptrJeu, ptrDesigner->listeJeuxParticipes);
        }
    }

    cout << "Le pointeur du jeu " << ptrJeu->titre << " a été détruit." << endl;
    delete[] ptrJeu->designers.elements;
}

void detruireListeJeux(ListeJeux& listeJeux)
{
    for (Jeu* ptrJeu : spanListeJeux(listeJeux))
    {
        detruirejeu(ptrJeu, listeJeux);
        delete ptrJeu;
    }
    delete[] listeJeux.elements;
}

void afficherDesigner(const Designer& ptrDesigner)
{
    cout << "\t" << ptrDesigner.nom << ", " << ptrDesigner.anneeNaissance << ", " << ptrDesigner.pays << endl;
    // TODO REMOVE CE QUI EST EN DESSOUS
    for (Jeu* ptrJeu : spanListeJeux(ptrDesigner.listeJeuxParticipes))
    {
        cout << "\t\t" << ptrJeu->titre << ", " << ptrJeu->developpeur << ", " << ptrJeu->anneeSortie << endl;
    }
}

void afficherJeu(const Jeu* ptrJeu)
{
    cout << ptrJeu->titre << ", " << ptrJeu->developpeur << ", " << ptrJeu->anneeSortie << endl;
    cout << "Designers: \n";

    for (Designer* ptrDesigner : spanListeDesigners(ptrJeu->designers))
    {
        afficherDesigner(*ptrDesigner);
    }
}

void afficherListeJeux(const ListeJeux& listeJeux)
{
    static const string ligneSeparation = "\n\033[12m--------------------------------------------------\033[0m\n";

    for (Jeu* ptrJeu : spanListeJeux(listeJeux))
    {
        afficherJeu(ptrJeu);
        cout << ligneSeparation;
    }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
#pragma region "Bibliothèque du cours"
    bibliotheque_cours::activerCouleursAnsi();
#pragma endregion

    static const string ligneSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";
    ListeJeux listeJeux = creerListeJeux("jeux.bin");

    // Tests pour les fonctions du main
    cout << ligneSeparation;
    cout << "Premier jeu de la liste :" << endl;
    afficherJeu(listeJeux.elements[0]);
    cout << ligneSeparation << endl;

    cout << "Affichage de la liste de jeu:\n" << endl;
    afficherListeJeux(listeJeux);
    designerParticipeJeu(listeJeux.elements[0], listeJeux.elements[0]->designers.elements[0]); // Appel pour la couverture de code (Pas veritablement uttilise)


    // Tests sur les methodes de la classe Developpeur
    cout << ligneSeparation;
    cout << "Tests des methodes de la classe Developpeur: " << endl;
    Developpeur dev;
    cout << dev.obtenirNom() << endl;

    Developpeur square("Square");
    cout << square.obtenirNom() << endl;
    cout << square.compterNombreJeux(listeJeux) << endl; // Square a developpe 3 jeux
    square.mettreDeveloppeurAJour(listeJeux);
    square.afficherJeuxDeveloppes(); // Les jeux developpes par Square sont Chrono Trigger, Final Fantasy VI et Secret of Mana


    // Tests sur les methodes de la classe ListeDeveloppeurs
    Developpeur nintendo("Nintendo");
    nintendo.mettreDeveloppeurAJour(listeJeux);

    cout << ligneSeparation;
    cout << "Tests des methodes de la classe ListeDeveloppeurs: " << endl;
    ListeDeveloppeurs listeDev;
    listeDev.afficher();
    listeDev.ajouterDeveloppeur(&square);
    listeDev.ajouterDeveloppeur(&nintendo);
    listeDev.afficher();

    listeDev.retirerDeveloppeur(&square);
    listeDev.afficher();
    cout << ligneSeparation;

    detruireListeJeux(listeJeux);
}
