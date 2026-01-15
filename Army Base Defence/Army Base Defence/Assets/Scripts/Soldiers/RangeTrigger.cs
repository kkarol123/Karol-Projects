using UnityEngine;
using System.Collections.Generic;

public class RangeTrigger : MonoBehaviour
{
    Soldier soldier;

    List<Transform> enemiesInRange = new List<Transform>();
    public Transform currentTarget;

    public bool hasRotatedOnce;

    void Awake(){
        soldier = transform.parent.GetComponent<Soldier>();
    }





    //Below is the code for the soldier to only track the first enemy which enters his range
    //Without the code below, the soldier will track the closest enemy in his range and not the furthest enemy, which is not proper tower-defense targeting logic

    void OnTriggerEnter2D(Collider2D other){
        if (other.CompareTag("Enemy") == false){
            return;
        } 

        enemiesInRange.Add(other.transform);

        if (currentTarget == null){
            SelectFurthestEnemy();
            hasRotatedOnce = false; //reset the bool when a new target has been gotten
        }
    }

    void Update(){
        if (soldier.isPlaced == false){
            return;
        }
        else{
            if (enemiesInRange.Count == 0){
                currentTarget = null;
                hasRotatedOnce = false;
                return;
            }

            SelectFurthestEnemy();

            Vector2 direction = currentTarget.position - soldier.transform.position;
            float angle = Mathf.Atan2(direction.y, direction.x) * Mathf.Rad2Deg + 90f;
            
            soldier.transform.rotation = Quaternion.Euler(0f, 0f, angle);

            hasRotatedOnce = true;
        }
    }

    void OnTriggerExit2D(Collider2D other){
        if (other.CompareTag("Enemy") == false){
            return;
        }

        enemiesInRange.Remove(other.transform);

        if (other.transform == currentTarget){
            SelectFurthestEnemy();
            hasRotatedOnce = false;
        }
    }



    void SelectFurthestEnemy(){
        currentTarget = null;
        int highestWaypoint = -1;

        foreach (Transform enemyTransform in enemiesInRange){
            Enemy enemy = enemyTransform.GetComponent<Enemy>();

            if (enemy.currentIndex > highestWaypoint){
                highestWaypoint = enemy.currentIndex;
                currentTarget = enemyTransform;
            }
        }
    }
}