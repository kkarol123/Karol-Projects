using UnityEngine;

public class OnBoxClicked : MonoBehaviour
{
    [SerializeField] SoldierData data;
    [SerializeField] int costToPlace;

    bool isPlacing = false;
    GameObject soldier;

    [SerializeField] LayerMask roadLayer;
    SpriteRenderer spriteRenderer;
    

    void OnMouseDown(){
        if (costToPlace > GameData.Money){
            return;
        }
        else{
            isPlacing = true;
            soldier = Instantiate(data.prefab, new Vector2(0f, 0f), Quaternion.identity);
            spriteRenderer = soldier.GetComponent<SpriteRenderer>();
        }
    }

    void Update(){
        if (isPlacing == false || costToPlace > GameData.Money){
            return;
        }
        else{
            Vector3 mouseScreenPos = Input.mousePosition;
            mouseScreenPos.z = -Camera.main.transform.position.z;
            Vector3 mouseWorldPos = Camera.main.ScreenToWorldPoint(mouseScreenPos);

            soldier.transform.position = new Vector2(mouseWorldPos.x, mouseWorldPos.y);

            //the below "onRoad" variable and overlapcircle method is to check if the soldier is going to be placed on the road, which is not possible
            bool onRoad = Physics2D.OverlapCircle(soldier.transform.position, 0.3f, roadLayer);
            if (spriteRenderer != null){
                spriteRenderer.color = onRoad ? Color.red : Color.white;
            }

            if(Input.GetMouseButtonUp(0)){             
                if (onRoad == true){
                    Destroy(soldier);
                    isPlacing = false;
                }
                else{
                    isPlacing = false;
                    GameData.Money -= costToPlace;

                    Soldier soldierScript = soldier.GetComponent<Soldier>();
                    soldierScript.isPlaced = true;

                    spriteRenderer.color = Color.white;

                    soldierScript.GetComponentInChildren<RangeTrigger>().gameObject.SetActive(true); //to start rotating
                    soldierScript.GetComponentInChildren<RangeTrigger>().enabled = true;
                }                
            }
        }
    }
}