using UnityEngine;
using System.Collections;

public class Soldier : MonoBehaviour{
    [SerializeField] SoldierData data;

    [SerializeField] GameObject bulletPrefab;
    [SerializeField] GameObject gunTip;
    
    RangeTrigger rangeTrigger;

    public bool isPlaced = false;

    Coroutine shootRoutine;

    void Awake(){
        rangeTrigger = GetComponentInChildren<RangeTrigger>();
    }

    void Update(){
        if (isPlaced == false){
            return;
        }
        else{
            if (rangeTrigger.currentTarget != null && rangeTrigger.hasRotatedOnce == true && shootRoutine == null){
                shootRoutine = StartCoroutine(ShootLoop());
            }
            else if (rangeTrigger.currentTarget == null && shootRoutine != null){
                StopCoroutine(shootRoutine);
                shootRoutine = null;
            }
        }
    }


    IEnumerator ShootLoop(){
        while (rangeTrigger.currentTarget != null){
            Instantiate(bulletPrefab, gunTip.transform.position, gunTip.transform.rotation);
            yield return new WaitForSeconds(1f / data.rateOfFire);
        }

        shootRoutine = null;
    }
}
