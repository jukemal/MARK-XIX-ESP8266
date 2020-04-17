class Medicine {
  constructor(name, price, description, slot, image_link, timestamp) {
    this.name = name;
    this.price = price;
    this.description = description;
    this.slot = slot;
    this.image_link = image_link;
    this.timestamp = timestamp;
  }
  toString() {
    return (
      this.name +
      ", " +
      this.price +
      ", " +
      this.description +
      ", " +
      this.slot +
      ", " +
      this.image_link +
      ", " +
      this.timestamp
    );
  }
}

toastr.options = {
  closeButton: true,
  positionClass: "toast-bottom-right",
  showDuration: "300",
  hideDuration: "1000",
  timeOut: "5000",
  extendedTimeOut: "1000",
  showEasing: "swing",
  hideEasing: "linear",
  showMethod: "fadeIn",
  hideMethod: "fadeOut",
};

var firebaseConfig = {
  apiKey: "AIzaSyBXGUoP1NbhhuJeoJA6cs9-51wMLQWJ6SE",
  authDomain: "mark-xix.firebaseapp.com",
  databaseURL: "https://mark-xix.firebaseio.com",
  projectId: "mark-xix",
  storageBucket: "mark-xix.appspot.com",
  messagingSenderId: "829696536097",
  appId: "1:829696536097:web:201d34c13bcb0b148e7d60",
  measurementId: "G-EFDM2B4EWK",
};
// Initialize Firebase
firebase.initializeApp(firebaseConfig);

let overlay = document.querySelector("#overlay");

overlay.style.display = "none";

const medicineList = {};
const selectedMedicineList = {};

var db = firebase.firestore();

db.collection("medicines").onSnapshot(function (querySnapshot) {
  let itemList = document.querySelector("#item-list");

  querySnapshot.forEach(function (doc) {
    medicine = new Medicine(
      doc.data().name,
      doc.data().price,
      doc.data().description,
      doc.data().slot,
      doc.data().image_link,
      doc.data().timestamp
    );

    medicineList[doc.id] = medicine;

    let bu = document.createElement("button");
    bu.type = "button";
    bu.className = "list-group-item list-group-item-action";
    bu.appendChild(document.createTextNode(medicineList[doc.id].name));
    console.log();

    itemList.appendChild(bu);

    bu.addEventListener("click", (e) => {
      if (e.target.style.background == "red") {
        e.target.style.background = "";
        delete selectedMedicineList[doc.id];
        console.log(JSON.stringify(selectedMedicineList));
      } else {
        e.target.style.background = "red";
        selectedMedicineList[doc.id] = medicineList[doc.id];
        console.log(JSON.stringify(selectedMedicineList));
      }
    });
  });
});

//console.log(document.domain);

document.getElementById("button").addEventListener("click", (e) => {
  overlay.style.display = "block";

  fetch(document.domain + "/medicines", {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify(selectedMedicineList),
  })
    .then((response) => response.json())
    .then((data) => {
      console.log("Success:", data);

      if (data.status == "Successful") {
        toastr.success("Order Successful.", "Success");
        overlay.style.display = "none";
      }
    })
    .catch((error) => {
      toastr.error("Error Processing Request.", "Error");
      overlay.style.display = "none";

      console.error("Error:", error);
    });
});
