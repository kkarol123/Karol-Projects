using UnityEngine;
using System.Collections.Generic;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using System.Linq;
using TMPro;
using System;

public class ShowHighScores : MonoBehaviour{
    [SerializeField] TMP_Text text1;
    [SerializeField] TMP_Text text2;
    [SerializeField] TMP_Text text3;


    void Start(){
        if (File.Exists(Application.persistentDataPath + "/highScores.dat") == false){
            text1.text = "No high scores found.";
            text2.text = "";
            text3.text = "";
        }
        else{
            BinaryFormatter bf = new BinaryFormatter();
            FileStream file = File.Open(Application.persistentDataPath + "/highScores.dat", FileMode.Open);

            //this is used for sorting below
            List<PlayerData> highScores =(List<PlayerData>)bf.Deserialize(file);

            file.Close();




            //Sort by health and then by money
            highScores.Sort((a, b) =>{
                if (a.health != b.health)
                    return b.health - a.health;   //higher health first

                return b.money - a.money;         //higher money first
            });

            if (highScores.Count > 0){
                text1.text = "1) HP: " + highScores[0].health + " | Money: " + highScores[0].money;
            }
            if (highScores.Count > 1){
                text2.text = "2) HP: " + highScores[1].health + " | Money: " + highScores[1].money;
            }
            if (highScores.Count > 2){
                text3.text = "3) HP: " + highScores[2].health + " | Money: " + highScores[2].money;
            }
        }
    }
}
