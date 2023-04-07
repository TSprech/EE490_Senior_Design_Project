// TSprech 2023/04/05 15:42:58

console.log("Renderer is running\n");

const para_text = document.getElementById("MyText");
function ChangeText(new_text: string) : void {
  if (para_text !== null) {
    para_text.innerHTML = new_text;
  } else {
    alert("Error: Could not find paragraph");
  }
}

ChangeText("CUSTOM TEXT");

const setButton = document.getElementById('btn')
if (setButton !== null) {
  setButton.addEventListener('click', () => {
    // @ts-ignore
    window.LED.TurnOn("This is the button text!");
  });
}
