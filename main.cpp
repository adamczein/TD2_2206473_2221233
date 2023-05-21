#include "Jeu.hpp"
#include <iostream>
#include <fstream>
#include "cppitertools/range.hpp"
#include "gsl/span"
#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp"
#include "debogage_memoire.hpp" //NOTE: Incompatible avec le "placement new", ne pas utiliser cette entête si vous utilisez ce type de "new" dans les lignes qui suivent cette inclusion.
#include "Developpeur.hpp"
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

//TODO: Fonction qui cherche un designer par son nom dans une ListeJeux.
// Cette fonction renvoie le pointeur vers le designer si elle le trouve dans
// un des jeux de la ListeJeux. En cas contraire, elle renvoie un pointeur nul.

Designer* trouverDesignerParNom(ListeJeux& listeJeux, string nomDesigner)
{

	for (Jeu* jeu : span<Jeu*>(listeJeux.elements, listeJeux.nElements))
	{	
		for (Designer* designer : span<Designer*>(jeu->designers.elements, jeu->designers.nElements))
	
		{ 
			if (designer->nom == nomDesigner)
			{
				return designer;
			}

		   
		}
	}

	return nullptr;	
}


Designer* lireDesigner(istream& fichier, ListeJeux& listeJeux)
{
	Designer designer = {}; // On initialise une structure vide de type Designer.
	designer.nom = lireString(fichier);
	designer.anneeNaissance = lireUint16(fichier);
	designer.pays = lireString(fichier);
	// Rendu ici, les champs précédents de la structure designer sont remplis
	// avec la bonne information.

	//TODO: Ajouter en mémoire le designer lu. Il faut renvoyer le pointeur créé.
	// Attention, valider si le designer existe déjà avant de le créer, sinon
	// on va avoir des doublons car plusieurs jeux ont des designers en commun
	// dans le fichier binaire. Pour ce faire, cette fonction aura besoin de
	// la liste de jeux principale en paramètre.

	Designer* designerRecherche = trouverDesignerParNom(listeJeux, designer.nom); 
		
	if (designerRecherche == nullptr)
		designerRecherche = new Designer(designer);

	cout << "L'allocation du designer " << designerRecherche->nom << "est réussie." << endl;
	return designerRecherche;
	 // Afficher un message lorsque l'allocation du designer est réussie.
	//cout << designer.nom << endl;  //TODO: Enlever cet affichage temporaire servant à voir que le code fourni lit bien les jeux.
			//sreturn nullptr;//TODO: Retourner le pointeur vers le designer crée.
}

//TODO: Fonction qui change la taille du tableau de jeux de ListeJeux.
// Cette fonction doit recevoir en paramètre la nouvelle capacité du nouveau
// tableau. Il faut allouer un nouveau tableau assez grand, copier ce qu'il y
// avait dans l'ancien, et éliminer l'ancien trop petit. N'oubliez pas, on copie
// des pointeurs de jeux. Il n'y a donc aucune nouvelle allocation de jeu ici !

void redimensionnerTableauJeux(ListeJeux& listeJeux, unsigned nouvelleCapacite)
{
	
	Jeu** nouveauTableauJeu = new Jeu * [listeJeux.capacite];

	for (unsigned i = 0; i < listeJeux.nElements; i++)
	{
		nouveauTableauJeu[i] = listeJeux.elements[i];
	}
	listeJeux.elements = nouveauTableauJeu;
	listeJeux.capacite = nouvelleCapacite;
	
}


//TODO: Fonction pour ajouter un Jeu à ListeJeux.
// Le jeu existant déjà en mémoire, on veut uniquement ajouter le pointeur vers
// le jeu existant. De plus, en cas de saturation du tableau elements, cette
// fonction doit doubler la taille du tableau elements de ListeJeux.
// Utilisez la fonction pour changer la taille du tableau écrite plus haut.

void ajouterJeu(ListeJeux& listeJeux, Jeu* jeu)
{	
	if (listeJeux.capacite == 0)
	{
		redimensionnerTableauJeux(listeJeux, 1);
	}


	if (listeJeux.nElements >= listeJeux.capacite)
	{
		unsigned nouvelleCapacite = listeJeux.capacite * 2;
		redimensionnerTableauJeux(listeJeux, nouvelleCapacite);
	}

	listeJeux.elements[listeJeux.nElements] = jeu;
	listeJeux.nElements++;

}



//TODO: Fonction qui enlève un jeu de ListeJeux.
// Attention, ici il n'a pas de désallocation de mémoire. Elle enlève le
// pointeur de la ListeJeux, mais le jeu pointé existe encore en mémoire.
// Puisque l'ordre de la ListeJeux n'a pas être conservé, on peut remplacer le
// jeu à être retiré par celui présent en fin de liste et décrémenter la taille
// de celle-ci.

void enleverJeu( ListeJeux& listeJeux,const Jeu* pointeurJeu)
{
	Jeu** elements = listeJeux.elements;
	unsigned int& nElements = listeJeux.nElements;

	for (int i = 0; i < nElements; i++)
	{
		if (elements[i] == pointeurJeu)
		{
			if (i != nElements - 1)
			{
				elements[i] = elements[nElements - 1];
			}
			nElements--;
			return;
		}
	}
}


void enleverDesigner(ListeDesigners& listeDesigner, Designer* designer)
{
	int index = 0;

	for (unsigned i = 0; i < listeDesigner.nElements; i++)
	{
		if (listeDesigner.elements[i] == designer)
		{
			break;
		}

		index++;

	}

	listeDesigner.elements[index] = listeDesigner.elements[listeDesigner.nElements - 1];
	listeDesigner.elements[listeDesigner.nElements - 1] = nullptr;
	listeDesigner.nElements--;

}

Jeu* lireJeu(istream& fichier, ListeJeux& listeJeux)
{
	Jeu jeu = {}; // On initialise une structure vide de type Jeu
	jeu.titre = lireString(fichier);
	jeu.anneeSortie = lireUint16(fichier);
	jeu.developpeur = lireString(fichier);
	jeu.designers.nElements = lireUint8(fichier);
	// Rendu ici, les champs précédents de la structure jeu sont remplis avec la
	// bonne information.

	//TODO: Ajouter en mémoire le jeu lu. Il faut renvoyer le pointeur créé.
	// Attention, il faut aussi créer un tableau dynamique pour les designers
	// que contient un jeu. Servez-vous de votre fonction d'ajout de jeu car la
	// liste de jeux participé est une ListeJeu. Afficher un message lorsque
	// l'allocation du jeu est réussie.

	Jeu* pointeurJeu = new Jeu(jeu);
	pointeurJeu->designers.elements = new Designer * [pointeurJeu->designers.nElements];
	

	//cout << jeu.titre << endl;  //TODO: Enlever cet affichage temporaire servant à voir que le code fourni lit bien les jeux.

	for ([[maybe_unused]] int i : iter::range(jeu.designers.nElements))
	{ 
		Designer* pointeurDesigner = lireDesigner(fichier, listeJeux);

		pointeurJeu->designers.elements[i] = pointeurDesigner;
		ajouterJeu(pointeurDesigner->listeJeuxParticipes, pointeurJeu);

	}

	cout << "L'allocation du jeu " << pointeurJeu->titre << "est réussie." << endl;
	return pointeurJeu; //TODO: Retourner le pointeur vers le nouveau jeu.
}

ListeJeux creerListeJeux(const string& nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);
	int nElements = lireUint16(fichier);
	ListeJeux listeJeux = {};
	listeJeux.elements = new Jeu * [nElements];
	listeJeux.capacite = nElements;

	for([[maybe_unused]] int n : iter::range(nElements))
	{
		Jeu* jeu = lireJeu(fichier, listeJeux);
		ajouterJeu(listeJeux, jeu);//TODO: Ajouter le jeu à la ListeJeux.
	}

	return listeJeux; //TODO: Renvoyer la ListeJeux.
}

//TODO: Fonction pour détruire un designer (libération de mémoire allouée).
// Lorsqu'on détruit un designer, on affiche son nom pour fins de débogage.

void detruireDesigner(Designer* designer)
{
	cout << "Le pointeur du designer " << designer->nom << " est détruit." << endl;
	delete[] designer->listeJeuxParticipes.elements;
	delete designer;
}


//TODO: Fonction qui détermine si un designer participe encore à un jeu.

bool designerParticipe(Designer* designer)
{
	if (designer->listeJeuxParticipes.elements != 0)
	{
		return true;
	}

	return false;
}

////TODO: Fonction pour détruire un jeu (libération de mémoire allouée).
//// Attention, ici il faut relâcher toute les cases mémoires occupées par un jeu.
//// Par conséquent, il va falloir gérer le cas des designers (un jeu contenant
//// une ListeDesigners). On doit commencer par enlever le jeu à détruire des jeux
//// qu'un designer a participé (listeJeuxParticipes). Si le designer n'a plus de
//// jeux présents dans sa liste de jeux participés, il faut le supprimer.  Pour
//// fins de débogage, affichez le nom du jeu lors de sa destruction.

void detruirejeu(Jeu* pointeurJeu, ListeJeux& listeJeu)
{
	for (Designer* pointeurDesigner : spanListeDesigners(pointeurJeu->designers))
	{
		enleverJeu(pointeurDesigner->listeJeuxParticipes, pointeurJeu);
		if (!designerParticipe(pointeurDesigner))
		{
			detruireDesigner(pointeurDesigner);
			
		}
	}

	cout << "Le pointeur du jeu " << pointeurJeu->titre << " a été détruit." << endl;
	delete[] pointeurJeu->designers.elements;
	delete pointeurJeu;
}

void detruireListeJeux(ListeJeux& listeJeux)
{
	for (Jeu* pointeurJeu : spanListeJeux(listeJeux))
	{
		detruirejeu(pointeurJeu, listeJeux);
		delete pointeurJeu;
	}
	delete[] listeJeux.elements;
}


void afficherDesigner(const Designer& d)
{
	cout << "\t" << d.nom << ", " << d.anneeNaissance << ", " << d.pays
			  << endl;
}

//TODO: Fonction pour afficher les infos d'un jeu ainsi que ses designers.
// Servez-vous de la fonction afficherDesigner ci-dessus.

void afficherJeu(const Jeu* jeu)
{
	cout << "Jeu: " << jeu->titre << endl;
	cout << "Annee de sortie: " << jeu->anneeSortie << endl;
	cout << "Développeur: " << jeu->developpeur << endl;
	cout << "Les designers: " << endl;
	for (Designer* designer : span <Designer*>(jeu->designers.elements, jeu->designers.nElements))
	{
		afficherDesigner(*designer);
	}
}

//TODO: Fonction pour afficher tous les jeux de ListeJeux, séparés par un ligne.
// Servez-vous de la fonction d'affichage d'un jeu crée ci-dessus. Votre ligne
// de séparation doit être différent de celle utilisée dans le main.

void afficherListeJeux (const ListeJeux& listeJeux)
{
	static const string ligneDeSeparation = "------------------------------------------------------------------------------------------------------------------------";
	cout <<ligneDeSeparation;
	
	for (Jeu* jeuAffiche : span<Jeu*>(listeJeux.elements, listeJeux.nElements)) {
		
		afficherJeu(jeuAffiche);
		cout << ligneDeSeparation << endl;
	}
}


int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	#pragma region "Bibliothèque du cours"
	// Permet sous Windows les "ANSI escape code" pour changer de couleur
	 https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac
	 //les supportent normalement par défaut.
	//bibliotheque_cours::activerCouleursAnsi(); 
	#pragma endregion

	int* fuite = new int;  // Pour vérifier que la détection de fuites fonctionne; un message devrait dire qu'il y a une fuite à cette ligne.

	ListeJeux listeJeux = creerListeJeux("jeux.bin"); //TODO: Appeler correctement votre fonction de création de la liste de jeux.

	static const string ligneSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";
	//TODO: Afficher le premier jeu de la liste (en utilisant la fonction).  Devrait être Chrono Trigger.
	cout << ligneSeparation << endl;
	cout << "Premier jeu de la liste : " << endl;
	cout << listeJeux.elements[0]->titre << endl;
	cout << ligneSeparation << endl;

	////TODO: Appel à votre fonction d'affichage de votre liste de jeux.
	////
	afficherListeJeux(listeJeux);

	////TODO: Faire les appels à toutes vos fonctions/méthodes pour voir qu'elles fonctionnent et avoir 0% de lignes non exécutées dans le programme (aucune ligne rouge dans la couverture de code; c'est normal que les lignes de "new" et "delete" soient jaunes).  Vous avez aussi le droit d'effacer les lignes du programmes qui ne sont pas exécutée, si finalement vous pensez qu'elle ne sont pas utiles.

	//
	////TODO: Détruire tout avant de terminer le programme.  Devrait afficher "Aucune fuite detectee." a la sortie du programme; il affichera "Fuite detectee:" avec la liste des blocs, s'il manque des delete.

	detruireListeJeux(listeJeux);
	
}
